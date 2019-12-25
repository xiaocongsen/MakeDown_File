# 介绍
第一步初始化 libevent 库，并保存返回的指针 
    event_init()  实际上这一步相当于初始化一个 Reactor 实例；在初始化 libevent 后，就可以注册事件了。
第二步调用函数
    void event_set(struct event *ev, int fd, short event, void (*cb)(int, short, void *), void *arg);
    ev：执行要初始化的 event 对象；
    fd：该 event 绑定的“句柄”，对于信号事件，它就是关注的信号；
    event：在该 fd 上关注的事件类型，它可以是 EV_READ, EV_WRITE, EV_SIGNAL； 一个事件被设置了EV_PERSIST，那么这个事件就是持续化的，意思就是这个事件会保持挂起状态，即使回调函数被执行。
    cb：这是一个函数指针，当 fd 上的事件 event 发生时，调用该函数执行处理，它有三个参数，
    分别是关注的fd, 关注的事件类型（读/写/信号），回调函数的参数void* arg，调用时由
    event_base 负责传入，按顺序，实际上就是 event_set 时的 fd, event 和 arg；
    arg：传递给 cb 函数指针的参数；
    由于定时事件不需要 fd，并且定时事件是根据添加时（ event_add）的超时值设定的，因此
    这里 event 也不需要设置。
    这一步相当于初始化一个 event handler，在 libevent 中事件类型保存在 event 结构体中。
    注意： libevent 并不会管理 event 事件集合，这需要应用程序自行管理；
第三步设置 event 从属的 event_base
    event_base_set(base, &ev);
    这一步相当于指明 event 要注册到哪个 event_base 实例上；
第四步 将事件添加到事件队列里
    event_add(&ev, timeout);
    基本信息都已设置完成，只要简单的调用 event_add()函数即可完成，其中 timeout 是定时值；
    这一步相当于调用 Reactor::register_handler()函数注册事件。
第五步 程序进入无限循环，等待就绪事件并执行事件处理
    event_base_dispatch(base);  
