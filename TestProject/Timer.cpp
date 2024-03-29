#include "Timer.h"
#include "communication.h"
#include "GameFramework.h"
extern SOCKET sock;
extern CGameFramework gGameFramework;
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
		m_nLastTime = ::timeGetTime();	//거의 쓰이지 않는 함수
		// #define LEAN_AND_MEAN으로 인해 짤린 함수, 이런 건 또 처음본다.
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

void CGameTimer::Tick(float fLockFPS)		
{	
	// 데이터 통신
	// CPlayer* p = new CPlayer;
	SendPacket(sock, gGameFramework.m_pPlayer);
	// RecvPacket(sock, p); 

	if (m_bHardwareHasPerformanceCounter)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)& m_nCurrentTime);
	}
	else
	{
		m_nCurrentTime = ::timeGetTime();
	}

	float fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
	//마지막으로 이 함수를 호출한 이후 경과한 시간을 계산한다.

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
			//이 함수의 파라메터(fLockFPS)가 0보다 크면 이 시간만큼 호출한 함수를 기다리게 한다.

			fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
			//마지막으로 이 함수를 호출한 이후 경과한 시간을 계산한다.
		}

	}

	m_nLastTime = m_nCurrentTime;
	//현재 시간을 m_nLastTime에 저장한다.

	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
	{
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT)m_nSampleCount++;
	}
	/*마지막 프레임 처리 시간과 현재 프레임 처리 시간의 차이가 1초보다 작으면
	현재 프레임 시간을 m_fFrameTime[0]에 저장한다.*/

	m_nFramesPerSecond++;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f)
	{
		m_nCurrentFrameRate = m_nFramesPerSecond;
		m_nFramesPerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}
	//초당 프레임 수를 1 증가시키고 현재 프레임 처리 시간을 누적하여 저장한다.

	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++)m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0)m_fTimeElapsed /= m_nSampleCount;
	//누적된 프레임 처리 시간의 평균을 구하여 프레임 처리 시간을 구한다.
}

unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	if (lpszString)
	{
		_itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T("FPS)"));
	}
	//현재 프레임 레이트를 문자열로 변환하여 lpszString 버퍼에 쓰고 "FPS"와 결합한다.

	return(m_nCurrentFrameRate);
}
//마지막 프레임 레이트를 문자열과 정수형으로 반환한다.

float CGameTimer::GetTimeElapsed()
{
	return(m_fTimeElapsed);
}