typedef int32_t  ostime_t;
struct osjob_t;  // fwd decl.

typedef void(*osjobcb_t) (struct osjob_t*);

struct osjob_t {
	struct osjob_t* next;
	ostime_t deadline;
	osjobcb_t  func;
};

class MyClass
{
public:
	MyClass();
	~MyClass();

private:
	osjob_t* scheduledjobs;
	osjob_t* runnablejobs;
	osjob_t timeoutjob;


public:
	osjob_t job;
	void tx_func(osjob_t* job);

	void os_runloop_once();
	void os_setCallback(osjob_t* job, osjobcb_t cb);
};

MyClass::MyClass()
{
}

MyClass::~MyClass()
{
}

void MyClass::os_runloop_once()
{
	osjob_t* j = NULL;
	if (runnablejobs) {
		j = runnablejobs;
		runnablejobs = j->next;
	}
	else if (scheduledjobs /*&& hal_checkTimer(OS.scheduledjobs->deadline)*/) {
		j = scheduledjobs;
		scheduledjobs = j->next;
	}
	else { 
		//hal_sleep(); 
	}
	if (j) { 
		j->func(j);
	}
}

void MyClass::os_setCallback(osjob_t* job, osjobcb_t cb)
{
	osjob_t** pnext;
	//os_clearCallback(job);
	job->func = cb;
	job->next = NULL;
	for (pnext = &runnablejobs; *pnext; pnext = &((*pnext)->next));
	*pnext = job;
}


MyClass test;

void setup()
{
	test.os_setCallback(&test.job, test.tx_func);
}

void loop()
{
	test.os_runloop_once();
}
