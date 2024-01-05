#include "EnginePCH.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool() : ThreadPool(thread::hardware_concurrency() * 2 + 1)
{
}

ThreadPool::ThreadPool(size_t numThreads)
{
	SetThreads(numThreads);
}

ThreadPool::~ThreadPool()
{
	TerminateAllThreads();
	Release();
}

void ThreadPool::AddJobFunction(const function<void()>& job)
{
	{
		unique_lock<mutex> lock(m_mutexThread);
		m_q.push(job);
		m_numJob.fetch_add(1, memory_order_relaxed);
	}
	m_cvThread.notify_one();
}

void ThreadPool::AddJobFunction(function<void()>&& job)
{
	{
		unique_lock<mutex> lock(m_mutexThread);
		m_q.push(job);
		m_numJob.fetch_add(1, memory_order_relaxed);
	}
	m_cvThread.notify_one();
}

unsigned int ThreadPool::GetNumJob() const
{
	return m_numJob.load(memory_order_relaxed);
}

bool ThreadPool::Empty() const
{
	return m_numJob.load(memory_order_relaxed) == 0;
}

void ThreadPool::Join()
{
	while (m_numJob.load(memory_order_relaxed) > 0);
}

void ThreadPool::SetThreads(size_t numThreads)
{
	m_numThreads = numThreads;
	m_threads = new thread[m_numThreads];

	for (size_t i = 0; i < m_numThreads; ++i)
	{
		thread _thread([&]() { Thread(); });
		m_threads[i].swap(_thread);
	}
}

void ThreadPool::TerminateAllThreads()
{
	unique_lock<mutex> lock(m_mutexThread);
	{
		queue<function<void()>> temp;
		m_q.swap(temp);
		m_terminate = true;
	}
	lock.unlock();

	m_cvThread.notify_all();

	for (size_t i = 0; i < m_numThreads; ++i)
	{
		thread& Thread = m_threads[i];
		if (Thread.joinable())
			Thread.join();
	}
}

void ThreadPool::Release()
{
	if (m_threads)
	{
		delete[] m_threads;
		m_threads = nullptr;
	}
	m_numThreads = 0;
}

void ThreadPool::Thread()
{
	while (1)
	{
		unique_lock<mutex> lock(m_mutexThread);
		m_cvThread.wait(lock, [&]() { return !m_q.empty() || m_terminate; });

		if (m_terminate)
		{
			return;
		}

		function<void()> func = std::move(m_q.front());
		m_q.pop();
		lock.unlock();

		func();

		m_numJob.fetch_sub(1, memory_order_relaxed);
	}
}
