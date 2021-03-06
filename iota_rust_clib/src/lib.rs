use iota_client::{Client, Error};
use std::ffi::CStr;
use std::ffi::CString;
use std::os::raw::c_char;
use serde::{Deserialize, Serialize};
//use serde_json::Result;

// https://github.com/serde-rs/json
#[derive(Serialize, Deserialize)]
struct JsonReturn {
    state: String,
    msg: Option<String>,
    message_id: Option<String>
}

#[no_mangle]
pub extern fn print_hello() 
{
    println!("Hello World");
}

fn my_string_safe(s: *const c_char) -> String {
    unsafe {
        CStr::from_ptr(s).to_string_lossy().into_owned()
    }
}


pub async fn send_message(iota_url: String, index: String, message_str: String) -> Result<String, Error>
{
    let iota = Client::builder()
        .with_node(&iota_url)?
        .with_local_pow(true)
        .finish()
        .await?;
    
    let message_bin_result = hex::decode(message_str);
    let message_bin = match message_bin_result {
        Ok(message) => message,
        Err(error) => panic!("Problem decode message from hex: {:?}", error),
    };
    
    let message = iota
        .message()
        .with_index(&index)
        //.with_data(hex::decode(message_str)?.as_bytes().to_vec())
        .with_data(message_bin.to_vec())
        .finish()
        .await?;

    Ok(message.id().0.to_string())
}

#[no_mangle]
pub extern fn iota_send_indiced_transaction(iota_url: *const c_char, index: *const c_char, message: *const c_char) -> *mut c_char 
{        
    let future = async move {
        // run actually function
        return send_message(my_string_safe(iota_url), my_string_safe(index), my_string_safe(message)).await;
    };
    
    // use tokio runtime to run and wait on async future
    let res = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap()
            .block_on(future);   
    // put result in json object 
    let json_result: JsonReturn; 
    match res {
        Ok(s) => {
            json_result= JsonReturn {
                state: "success".to_string(),
                msg: None,
                message_id: Some(s)
            }
        }
        Err(error) => {
            json_result= JsonReturn {
                state: "error".to_string(),
                msg: Some(error.to_string()),
                message_id: None
            }
        }
    }
   // Serialize it to a JSON string.
   let j = serde_json::to_string(&json_result);
   let c_string: CString;
   match j {
       Ok(j) => {
            c_string = CString::new(j).expect("CString::new failed");
       }
       Err(error) => {
            c_string = CString::new(format!("error with encoding result json: {}", error)).expect("CString::new failed");
       }
   }
   // return as char*, accessable from c, but it must be freed with a call to free_rust_string afterwards
   return c_string.into_raw();
}

#[no_mangle]
pub extern fn free_rust_string(rust_string: *mut c_char)
{
    unsafe {
        let _ = CString::from_raw(rust_string);
    }
}
