/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/// provide C interface to C++ for calling
pub mod ffi;
use hilog_rust::{info, error, hilog, HiLogLabel, LogType};
use std::{ptr, ffi::{CString, c_char}};
use libc::c_int;
const ONCE_PROCESS_NETPACKET_LIMIT: i32 = 100;
const MAX_PACKET_BUF_SIZE: usize = 256;
const SEND_RETRY_SLEEP_TIME: u64 = 10000;
const SEND_RETRY_LIMIT: i32 = 32;
const RET_ERR: i32 = -1;

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "EpollManager"
};

/// struct EpollManager
#[repr(C)]
pub struct EpollManager {
    /// socket_fd
    pub socket_fd: i32,
    /// epoll_fd
    pub epoll_fd: i32,
}

impl Default for EpollManager {
    fn default() -> Self {
        Self {
            socket_fd: -1,
            epoll_fd: -1,
        }
    }
}

impl EpollManager {
    /// return const referance of self
    ///
    /// # Safety
    /// 
    /// Makesure object is null pointer
    unsafe fn as_ref<'a>(object: *const Self) -> Option<&'a Self>{
        object.as_ref()
    }
    /// return mutable referance of self
    ///
    /// # Safety
    /// 
    /// Makesure object is null pointer
    unsafe fn as_mut<'a>(object: *mut Self) -> Option<&'a mut Self>{
        object.as_mut()
    }
    /// get socket_fd
    pub fn socket_fd(&self) -> i32 {
        self.socket_fd
    }
    /// get epoll_fd
    pub fn epoll_fd(&self) -> i32 {
        self.epoll_fd
    }
    /// adjust whether epoll_fd is valid or not 
    pub fn is_valid_epoll(&self) -> bool {
        self.epoll_fd > 0
    }
    /// create epoll_fd
    pub fn create_epoll_fd(&mut self, size: i32) {
        let epoll_fd;
        unsafe {
            epoll_fd = libc::epoll_create(size);
        }
        if epoll_fd < 0 {
            error!(LOG_LABEL, "epoll_create return {}", epoll_fd);
        } else {
            self.epoll_fd = epoll_fd;
            info!(LOG_LABEL, "epoll_create, epoll_fd:{}", epoll_fd);
        }
    }
    /// event is null pointer
    pub fn epoll_ctl(fd: i32, op: i32, event: *mut libc::epoll_event, epoll_fd: i32) -> i32 {
        if event.is_null() {
            error!(LOG_LABEL, "event is nullptr");
            return RET_ERR;
        }
        if op == libc::EPOLL_CTL_DEL {
            // safety: call unsafe function
            unsafe {
                libc::epoll_ctl(epoll_fd as c_int, op as c_int, fd as c_int, ptr::null_mut())
            }
        } else {
            // safety: call unsafe function
            unsafe {
                libc::epoll_ctl(epoll_fd as c_int, op as c_int, fd as c_int, event as *mut libc::epoll_event)
            }
        }
    }
    /// epoll_wait
    ///
    /// # Safety
    /// 
    /// call libc
    pub unsafe fn epoll_wait(events: *mut libc::epoll_event, maxevents: i32, timeout: i32, epoll_fd: i32) -> i32 {
        let ret = libc::epoll_wait(
            epoll_fd as c_int, events as *mut libc::epoll_event, maxevents as c_int, timeout as c_int);
        if ret < 0 {
            let errno = *libc::__errno_location();
            error!(LOG_LABEL, "epoll_wait ret:{},errno:{}", ret, errno);
        }
        ret
    }
    /// epoll close
    pub fn epoll_close(&mut self) {
        if self.epoll_fd >= 0 {
            unsafe {
                libc::close(self.epoll_fd as c_int);
            }
            self.epoll_fd = -1;
        }
    }
    /// socket_set_fd
    pub fn socket_set_fd(&mut self, fd: i32) {
        self.socket_fd = fd
    }
    /// socket close
    pub fn socket_close(&mut self) {
        if self.socket_fd >= 0 {
            unsafe {
                let res = libc::close(self.socket_fd as c_int);
                if res > 0 {
                    error!(LOG_LABEL, "Socket close failed res:{}", res);
                }
            }
            self.socket_fd = -1;
        }
    }
}


