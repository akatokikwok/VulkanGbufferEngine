#include <windows.h>

// ʶ���¼���Ϣ���͵Ļص�����
LRESULT CALLBACK LearnWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_CLOSE: // ��������رհ�ť��ʱ��,�¼��ᱻʶ��ΪWM_CLOSE��
			PostQuitMessage(0);// ����ϢΪWM_CLOSE�;��˳�ѭ��
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);// û����Ϣ�¼�����ʱ,��ʹ��Ĭ�ϵĴ���
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndclassex;
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.cbClsExtra = 0;// ���ڵĶ���ռ�
	wndclassex.cbWndExtra = 0;
	wndclassex.hbrBackground = nullptr;//������������Ⱦ������,����Ҫwindows gdiȥ��
	wndclassex.hCursor = LoadCursor(nullptr, IDC_ARROW);// ��ͷ���
	wndclassex.hIcon = nullptr;// �����ϵ�logo
	wndclassex.hIconSm = nullptr;// ��������ʱ���Ͻǵ�logo
	wndclassex.hInstance = hInstance;
	wndclassex.lpfnWndProc = LearnWindowProc;// �����¼��õĻص�����,�����������¼�
	wndclassex.lpszMenuName = nullptr;// û�в˵�
	wndclassex.lpszClassName = L"BattleFireWindow"; // ��L���ַ���charת��ΪWChar��,ע��ʹ��Unicode�ַ���
	wndclassex.style = CS_VREDRAW | CS_HREDRAW;// ����ʱ����ˮƽ�ػ�ʹ�ֱ�ػ�

	RECT rect = { 0, 0, 1280, 720 };// ����һ�����ڲ������߿�����ݴ�С,�������߿����ʾ����Ϊ1280*720,�������˱߿�����Ҫ��һ�еķ����Զ����ڼ���
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	ATOM atom = RegisterClassEx(&wndclassex);// ע�ᴰ��,����ATOM��
	if (atom == 0) {
		MessageBox(nullptr, L"��������:����ע��ʧ��", L"���ڱ���:����", MB_OK);
		return -1;
	}
	/* ��������*/
	HWND hwnd = CreateWindowEx(NULL, L"BattleFireWindow", L"LearnWindow", WS_OVERLAPPEDWINDOW, 
		0, 0,/*�������Ͻ�����*/ rect.right - rect.left, rect.bottom - rect.top,
		nullptr,/*�����ھ��*/ nullptr,/*�˵�*/ hInstance, nullptr/*��������ʱ����Ĳ���*/
	);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);// ��ʾ���ں���ˢ�³�Ҫ����ɫ

	MSG msg;//Ҫ�����Ϣ�Ľṹ��
	while (true) {
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {// ��ȡ������Ϣ���ٰ����ǴӶ������Ƴ���
			if (msg.message == WM_QUIT) {// ����ȡ������Ϣ����ʶ��ΪWM_QUIT���ж�ѭ��
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);// �ɷ���Ϣ,ʵ���Ͼ����õĴ��ڽṹ�����Ǹ�lpfnWndProc�����ķ���
		}		
	}
	return 0;
}

