use core::ffi::c_void;
use core::mem::MaybeUninit;
use core::ptr;
use core::cmp;

use heapless::spsc::{Queue, Producer, Consumer};
use spin::Mutex; // Using spin Mutex for simple locking in no_std

const QUEUE_CAPACITY: usize = 8; // Define the fixed size for the queue

#[repr(C)]
#[derive(Copy, Clone)]
pub struct CommandMessage {
    pub command_id: i32,
    pub data: [u8; 64],
}

// --- Structure to hold the FIFO state inside the Mutex ---
struct FifoState {
    queue: MaybeUninit<Queue<CommandMessage, QUEUE_CAPACITY>>,
    producer: MaybeUninit<Producer<'static, CommandMessage, QUEUE_CAPACITY>>,
    consumer: MaybeUninit<Consumer<'static, CommandMessage, QUEUE_CAPACITY>>,
    initialized: bool,
}

impl FifoState {
    const fn new() -> Self {
        FifoState {
            queue: MaybeUninit::uninit(),
            producer: MaybeUninit::uninit(),
            consumer: MaybeUninit::uninit(),
            initialized: false,
        }
    }
}

// --- Static Mutex containing the FIFO state ---
// SAFETY: Access to the inner fields requires locking the mutex.
// The 'static lifetime for Producer/Consumer is obtained via unsafe split below.
static FIFO_STATE: Mutex<FifoState> = Mutex::new(FifoState::new());

// A non-null pointer used simply as a success indicator handle by C++
const SUCCESS_HANDLE: *mut c_void = 1 as *mut c_void;

// --- FFI Functions ---

#[unsafe(no_mangle)]
pub unsafe extern "C" fn init_fifo() -> *mut c_void {
    let mut state = FIFO_STATE.lock(); // Lock the state

    if state.initialized {
        // Already initialized
        return ptr::null_mut();
    }

    // Initialize the queue within the locked state
    // SAFETY: We hold the lock, and state is not initialized.
    let queue_ptr = state.queue.as_mut_ptr();
    unsafe {
        queue_ptr.write(Queue::new());
    }

    // Split the queue into producer/consumer.
    // SAFETY: We need 'static lifetimes for the producer/consumer because they
    // will be stored back into the static state. This is safe because the state
    // (and thus the queue) is static and lives forever. We manage access via the lock.
    let queue_ref = unsafe { &mut *queue_ptr };
    let (producer, consumer) = queue_ref.split();

    // Store producer and consumer back into the locked state
    // SAFETY: We hold the lock, state is not initialized.
    unsafe {
        state.producer.as_mut_ptr().write(producer);
        state.consumer.as_mut_ptr().write(consumer);
    }

    state.initialized = true;
    SUCCESS_HANDLE // Return non-null handle on success
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn read_fifo(fifo_handle: *mut c_void) -> CommandMessage {
    if fifo_handle.is_null() {
         return CommandMessage { command_id: -3, data: [0; 64] }; // Indicate invalid handle state
    }

    let mut state = FIFO_STATE.lock(); // Lock the state

    if !state.initialized {
        // Attempting to read from uninitialized/destroyed FIFO
        return CommandMessage { command_id: -3, data: [0; 64] };
    }

    // SAFETY: We hold the lock, and state is initialized.
    let consumer = unsafe { &mut *state.consumer.as_mut_ptr() };

    match consumer.dequeue() {
        Some(msg) => msg,
        None => CommandMessage { command_id: -1, data: [0; 64] }, // FIFO empty
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn simulate_interrupt(fifo_handle: *mut c_void, command_id: i32, data: *const u8, data_len: usize) -> i32 {
     if fifo_handle.is_null() {
         return -2; // Indicate invalid handle state
     }

    let mut state = FIFO_STATE.lock(); // Lock the state

    if !state.initialized {
        // Attempting to write to uninitialized/destroyed FIFO
        return -2;
    }

    // SAFETY: We hold the lock, and state is initialized.
    let producer = unsafe { &mut *state.producer.as_mut_ptr() };

    // Prepare data
    let data_slice = unsafe { core::slice::from_raw_parts(data, data_len) };
    let mut data_array = [0u8; 64];
    let len_to_copy = cmp::min(data_len, 64);
    data_array[..len_to_copy].copy_from_slice(&data_slice[..len_to_copy]);

    let msg = CommandMessage { command_id, data: data_array };

    match producer.enqueue(msg) {
        Ok(_) => 0, // Return 0 on success
        Err(_) => -1, // Return -1 on error (queue full)
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn destroy_fifo(fifo_handle: *mut c_void) {
    if fifo_handle.is_null() {
        return; // Nothing to destroy
    }

    let mut state = FIFO_STATE.lock(); // Lock the state

    if !state.initialized {
        return; // Already destroyed or never initialized
    }

    // Explicitly drop the producer and consumer stored in the state
    // SAFETY: We hold the lock, and state is initialized.
    unsafe {
        ptr::drop_in_place(state.producer.as_mut_ptr());
        ptr::drop_in_place(state.consumer.as_mut_ptr());
        // Drop the queue itself to be thorough, although not strictly necessary
        // for Copy types if we were only resetting the initialized flag.
        ptr::drop_in_place(state.queue.as_mut_ptr());
    }

    // Mark as uninitialized
    state.initialized = false;
    // Reset MaybeUninit fields for potential re-initialization (optional but clean)
    state.queue = MaybeUninit::uninit();
    state.producer = MaybeUninit::uninit();
    state.consumer = MaybeUninit::uninit();
}
