#include "Timer.h"
#include "communication.h"
CGameTimer::CGameTimer()
{
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)& m_nPerformanceFrequency))
	{
		m_bHardwareHasPerformanceCounter = TRUE;
		::QueryPerformanceCounter((LARGE_INTEGER*)& m_nLastTime);
		m_fTimeScale = 1.0f / m_nPerformanceFrequency;
	}
	else
	{
		m_bHardwareHasPerformanceCounter = FALSE;
		m_nLastTime = ::timeGetTime();	//���� ������ �ʴ� �Լ�
		// #define LEAN_AND_MEAN���� ���� ©�� �Լ�, �̷� �� �� ó������.
		m_fTimeScale = 0.001f;
	}

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_nFramesPerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}


CGameTimer::~CGameTimer()
{
}

void CGameTimer::Tick(float fLockFPS)		// ���⼭ ������ ���
{	
	if (m_bHardwareHasPerformanceCounter)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)& m_nCurrentTime);
	}
	else
	{
		m_nCurrentTime = ::timeGetTime();
	}

	float fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
	//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�.

	if (fLockFPS > 0.0f)
	{
		while (fTimeElapsed < (1.0f / fLockFPS))
		{
			if (m_bHardwareHasPerformanceCounter)
			{
				::QueryPerformanceCounter((LARGE_INTEGER*)& m_nCurrentTime);
			}
			else
			{
				m_nCurrentTime = ::timeGetTime();
			}
			//�� �Լ��� �Ķ����(fLockFPS)�� 0���� ũ�� �� �ð���ŭ ȣ���� �Լ��� ��ٸ��� �Ѵ�.

			fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
			//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�.
		}

	}

	m_nLastTime = m_nCurrentTime;
	//���� �ð��� m_nLastTime�� �����Ѵ�.

	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
	{
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT)m_nSampleCount++;
	}
	/*������ ������ ó�� �ð��� ���� ������ ó�� �ð��� ���̰� 1�ʺ��� ������
	���� ������ �ð��� m_fFrameTime[0]�� �����Ѵ�.*/

	m_nFramesPerSecond++;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f)
	{
		m_nCurrentFrameRate = m_nFramesPerSecond;
		m_nFramesPerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}
	//�ʴ� ������ ���� 1 ������Ű�� ���� ������ ó�� �ð��� �����Ͽ� �����Ѵ�.

	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++)m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0)m_fTimeElapsed /= m_nSampleCount;
	//������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ���Ѵ�.
}

unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	if (lpszString)
	{
		_itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T("FPS)"));
	}
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� "FPS"�� �����Ѵ�.

	return(m_nCurrentFrameRate);
}
//������ ������ ����Ʈ�� ���ڿ��� ���������� ��ȯ�Ѵ�.

float CGameTimer::GetTimeElapsed()
{
	return(m_fTimeElapsed);
}