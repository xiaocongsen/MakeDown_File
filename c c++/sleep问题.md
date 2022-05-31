# sleep
注意：*sleep/select会受信号量(signal)影响提前退出，所以应用程序看到*sleep/select返回还要确认是正常返回（等待时间到达）还是中断返回。
如果中断返回，sleep返回剩余等待时间，usleep、nanosleep和select返回-1加errno=EINTR