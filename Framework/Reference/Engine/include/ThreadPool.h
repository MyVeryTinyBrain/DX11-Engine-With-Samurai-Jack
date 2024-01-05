#pragma once

ENGINE_BEGIN

class ENGINE_API ThreadPool
{
public:

	ThreadPool();
	ThreadPool(size_t numThreads);
	~ThreadPool();

public:

	void AddJobFunction(const function<void()>& job);
	void AddJobFunction(function<void()>&& job);

	unsigned int GetNumJob() const;
	bool Empty() const;
	void Join();

public:

	void SetThreads(size_t numThreads);
	void TerminateAllThreads();
	void Release();

private:

	void Thread();

private:

	size_t					m_numThreads = 0;
	thread*					m_threads = nullptr;
	mutex					m_mutexThread;
	condition_variable		m_cvThread;
	bool					m_terminate = false;
	queue<function<void()>>	m_q;
	atomic<unsigned int>	m_numJob = 0;
};

ENGINE_END
