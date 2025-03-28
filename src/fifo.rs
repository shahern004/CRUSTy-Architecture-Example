use std::sync::mpsc::{channel, Receiver, Sender, TryRecvError};
use std::sync::{Arc, Mutex};

#[repr(C)]
pub struct CommandMessage {
    pub command_id: i32,
    pub data: [u8; 64],
}

mod fifo_functions {
    use super::*;
        #[unsafe(no_mangle)]
        pub extern "C" fn init_fifo() -> *mut Arc<Mutex<(Sender<CommandMessage>, Receiver<CommandMessage>)>> {
            let (tx, rx) = channel();
            let pair = Arc::new(Mutex::new((tx, rx)));
            Box::into_raw(Box::new(pair))
        }

        #[unsafe(no_mangle)]
        pub extern "C" fn read_fifo(fifo_pair: *mut Arc<Mutex<(Sender<CommandMessage>, Receiver<CommandMessage>)>>) -> CommandMessage {
            let fifo_pair = unsafe { &mut *fifo_pair };
            match fifo_pair.lock().unwrap().1.try_recv() {
                Ok(msg) => msg,
                Err(TryRecvError::Empty) => CommandMessage { command_id: -1, data: [0; 64] },
                Err(TryRecvError::Disconnected) => CommandMessage { command_id: -2, data: [0; 64] },
            }
        }

        #[unsafe(no_mangle)]
        pub extern "C" fn simulate_interrupt(fifo_pair: *mut Arc<Mutex<(Sender<CommandMessage>, Receiver<CommandMessage>)>>, command_id: i32, data: *const u8, data_len: usize) -> i32 {
            let fifo_pair = unsafe { &mut *fifo_pair };
            let data_slice = unsafe { std::slice::from_raw_parts(data, data_len) };
            let mut data_array = [0u8; 64];
            data_array.copy_from_slice(&data_slice[..std::cmp::min(data_len, 64)]); // Handle data larger than 64 bytes

            fifo_pair.lock().unwrap().0.send(CommandMessage { command_id, data: data_array }).unwrap_or_else(|e| {
                println!("Error sending message to FIFO: {:?}", e);
            });
            0
        }

        #[unsafe(no_mangle)]
        pub extern "C" fn destroy_fifo(fifo_pair: *mut Arc<Mutex<(Sender<CommandMessage>, Receiver<CommandMessage>)>>) {
            unsafe {
                drop(Box::from_raw(fifo_pair));
            }
        }
    }