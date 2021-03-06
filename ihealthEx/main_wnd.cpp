#include "main_wnd.h"

#include <windows.h>
#include <random>

using namespace DuiLib;

LPCTSTR CMainWnd::GetWindowClassName() const {
	return _T("CMainWnd");
}

CDuiString CMainWnd::GetSkinFile() {
	return _T("main_wnd.xml");
}

void CMainWnd::InitWindow() {
	m_pInitialBtn = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnInitial")));
	m_pPositiveBtn = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnPositive")));
	m_pNegativeBtn = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnNegative")));
	m_pStopBtn = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnStop")));
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);
	m_pControlCard = new ControlCard();
	m_pFatigueTest = new FatigueTest();
}

void CMainWnd::Notify(TNotifyUI &msg) {
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("click")) {
		if (name.CompareNoCase(_T("btnInitial")) == 0) {
			m_pFatigueTest->Initial(m_hWnd);
		} else if (name.CompareNoCase(_T("btnPositive")) == 0) {
			m_pFatigueTest->StartTest();
			//m_pFatigueTest->AbsoluteMove();
		} else if (name.CompareNoCase(_T("btnNegative")) == 0) {
			m_pFatigueTest->PositionReset();
		} else if (name.CompareNoCase(_T("btnStop")) == 0) {
			m_pFatigueTest->StopMove();
		}
	}
}

void CMainWnd::OnFinalMessage(HWND hWnd) {
	if (m_pFatigueTest->IsInitialed()) {
		m_pFatigueTest->StopMove();
		delete m_pFatigueTest;
	}
}

CControlUI *CMainWnd::CreateControl(LPCTSTR pstrClassName) {
	if (_tcsicmp(pstrClassName, _T("Wnd")) == 0) {
		return InitializeChart();
	}
	return NULL;
}

LRESULT CMainWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg) {
		case CCHART_UPDATE:
			OnDataUpdate(lParam);
			break;
		default:
			bHandled = false;
			break;
	}
	if (bHandled) return lRes;
	if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMainWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	bHandled = FALSE;
	if (uMsg == WM_KEYDOWN) {
		if (wParam == VK_ESCAPE) {
			if (m_pFatigueTest->IsInitialed()) {
				m_pFatigueTest->StopMove();
				delete m_pFatigueTest;
			}
			::PostQuitMessage(0L);
			return true;
		}
	}
	return 0;
}

CControlUI *CMainWnd::InitializeChart() {
	double x[1] { 0 };
	double y[1] { 0 };
	CWndUI *pUI = new CWndUI;
	m_UIWnd = CreateWindow(_T("BUTTON"), _T("win32"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 0, 32, 1920, 1000, m_pm.GetPaintWindow(), NULL, NULL, NULL);
	pUI->Attach(m_UIWnd);
	m_ChartWnd.Attach(m_UIWnd, kTypeSplit);
	m_ChartWnd.GetChart()->ResizePlots(1, 3, 2);

	CChart *chart = m_ChartWnd.GetChart();
	chart->AddCurve(x, y, 1, 0);
	chart->AddCurve(x, y, 1, 1);
	chart->AddCurve(x, y, 1, 2);
	chart->AddCurve(x, y, 1, 3);
	chart->AddCurve(x, y, 1, 4);
	chart->AddCurve(x, y, 1, 5);
	/*chart->AddCurve(x, y, 1, 6);
	chart->AddCurve(x, y, 1, 7);
	chart->AddCurve(x, y, 1, 8);
	chart->AddCurve(x, y, 1, 9);*/

	chart->SetTitle(_T("fx"), 0);
	chart->SetTitle(_T("fy"), 1);
	chart->SetTitle(_T("fz"), 2);
	chart->SetTitle(_T("Mx"), 3);
	chart->SetTitle(_T("My"), 4);
	chart->SetTitle(_T("Mz"), 5);
	//chart->SetTitle(_T("肘部反向拉力"), 6);
	//chart->SetTitle(_T("肩部反向拉力"), 7);
	//chart->SetTitle(_T("肘部力矩"), 8);
	//chart->SetTitle(_T("肩部力矩"), 9);

	return pUI;
}

void CMainWnd::OnDataUpdate(LPARAM lParam) {
	FatigueTest *mTest = reinterpret_cast<FatigueTest *>(lParam);
	int data_id = m_ChartWnd.GetChart()->GetDataID(0);
	m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->elbow_angle_error, 500, 0);
	m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->shoulder_angle_error, 500, 1);
	m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->elbow_angle_curve, 500, 2);
	m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->shoulder_angle_curve, 500, 3);
	m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->pull_force_curve1, 500, 4);
	m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->pull_force_curve2, 500, 5);
	//m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->pull_force_curve3, 500, 6);
	//m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->pull_force_curve2, 500, 7);
	//m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->elbow_torque_curve, 500, 8);
	//m_ChartWnd.GetChart()->UpdateCurveByID(data_id, mTest->x_axis, mTest->shoulder_torque_curve, 500, 9);
	m_ChartWnd.ReDraw();
	/*RECT rt;
	GetClientRect(m_UIWnd, &rt);
	InvalidateRect(m_UIWnd, &rt, TRUE);*/
}