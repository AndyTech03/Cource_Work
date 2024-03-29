#include "Define.h"
#include "Point2.h"
#include "Part.h"
#include "Color.h"
#include "Window_Args.h"

HANDLE cons = GetStdHandle(STD_OUTPUT_HANDLE);
void Draw_Parts(Window_Args args)
{
	for (Part* p = &args.parts[0]; p < &args.parts[args.parts_count]; p++)
	{
		Point2 draw_pos = p->Position - p->Center + Point2(1, args.Scroll + 1);
		for (int y = 0; y < p->Size.Y; y++)
		{
			if (draw_pos.Y < 0 + 1 || draw_pos.Y > args.Size.Y - 2)
			{
				draw_pos.Y++;
				continue;
			}
			for (int x = 0; x < p->Size.X - 1; x++)
			{
				if (draw_pos.X + x < 1 || draw_pos.X + x > args.Size.X - 2)
				{
					continue;
				}
				SetConsoleCursorPosition(cons, draw_pos + Point2(x));
				cout << p->Sprite[y][x];
			}
			draw_pos.Y++;
		}
	}
}

void Update_Window(Window_Args& args)
{
	SetConsoleCursorPosition(cons, Point2());
	Point2 draw_pos;
	bool is_selection_color = false;
	if (args.connector_selecting)
	{
		if (args.parts_count == 0)
			args.part_selecting = true;
		else
		{
			for (int y = 0; y < args.Size.Y; y++)
			{
				for (int x = 0; x < args.Size.X; x++)
				{
					if (y == 0 || y == args.Size.Y - 1)
						cout << '-';
					else if (x == 0 || x == args.Size.X - 1)
						cout << '|';
					else
						cout << ' ';
				}
				cout << "\n";
			}
			Draw_Parts(args);
		}
		args.Move_Cursor();
		return;
	}

	if (args.part_selecting)
	{
		Window_Args::Window_Parts_Panel_Args panel_args = args.Parts_Panel_Args;
		draw_pos = Point2(panel_args.parts_padding+1, panel_args.parts_padding);

		// Clear part`s window place
		SetConsoleCursorPosition(cons, draw_pos);
		for (int y = 0; y < panel_args.part_window_size.Y; y++)
		{
			for (int x = 0; x < panel_args.part_window_size.X+1; x++)
			{
				if (x == panel_args.part_window_size.X)
				{
					cout << ' ';
					continue;
				}
				char c = ' ';
				if (y == 0 || y == panel_args.part_window_size.Y - 1)
					c = '-';
				else if (x == 0 || x == panel_args.part_window_size.X - 1)
					c = '|';
				cout << c;
			}
			SetConsoleCursorPosition(cons, Point2(draw_pos.X, draw_pos.Y+y+1));
		}
		draw_pos = draw_pos + Point2(2, 1);

		// Draw panel Head
		SetConsoleCursorPosition(cons, draw_pos);
		cout << " �������� ������:";
		draw_pos.Y++;
		SetConsoleCursorPosition(cons, draw_pos);
		cout << " (������� " << panel_args.page + 1 << " �� " << panel_args.pages_count << ')';
		draw_pos.Y++;
		SetConsoleCursorPosition(cons, draw_pos-Point2(2));
		for (int x = 0; x < panel_args.part_window_size.X; x++)
			cout << '-';

		// Add padding
		draw_pos.Y += 1 + panel_args.parts_padding;

		for (int i = panel_args.page * panel_args.parts_on_page; i < (panel_args.page + 1) * panel_args.parts_on_page; i++)
		{
			Point2 padding = Point2(panel_args.part_icon_size.X / 2 - panel_args.Parts[i].Center.X + 1, panel_args.part_icon_size.Y / 2 - panel_args.Parts[i].Center.Y);

			// Set selection color
			SetConsoleCursorPosition(cons, draw_pos);
			if (panel_args.selected_index == i && is_selection_color == false)
			{
				SetConsoleTextAttribute(cons, (WORD)Color::Green);
				is_selection_color = true;
			}

			// Draw top border
			for (int x = 0; x < panel_args.part_icon_size.X; x++)
				cout << '-';
			draw_pos.Y++;
			string title = Get_PartTitle(panel_args.Parts[i].Type);
			SetConsoleCursorPosition(cons, draw_pos + Point2((panel_args.part_icon_size.X - title.length())/2));
			cout << title;

			for (int y = 0; y < panel_args.part_icon_size.Y; y++, draw_pos.Y++)
			{
				// Left border
				SetConsoleCursorPosition(cons, draw_pos);
				cout << '|';

				// Right border
				SetConsoleCursorPosition(cons, Point2(draw_pos.X + panel_args.part_icon_size.X-1, draw_pos.Y));
				cout << '|';

				// Try draw part`s row
				int row = y - padding.Y-1;
				if (row >= 0 && row < panel_args.Parts[i].Size.Y && i < panel_args.parts_count)
				{
					SetConsoleCursorPosition(cons, Point2(draw_pos.X + padding.X + panel_args.parts_padding-1, draw_pos.Y));
					cout << panel_args.Parts[i].Sprite[row];
				}
			}

			// Draw bottom border
			SetConsoleCursorPosition(cons, draw_pos);
			for (int x = 0; x < panel_args.part_icon_size.X; x++)
				cout << '-';

			// Add padding
			draw_pos.Y += 1 + panel_args.parts_padding;

			// UnSet selection color
			SetConsoleCursorPosition(cons, draw_pos);
			if (is_selection_color == true)
			{
				SetConsoleTextAttribute(cons, (WORD)Color::White);
				is_selection_color = false;
			}
		}
	}
}
string convertWstringToAstringWinapi(const  std::wstring& src)   throw (std::runtime_error)
{
	if (src.length() < 1024)
	{
		char temp[1048];
		temp[0] = '\0';

		int winapires = WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, temp, 1048, NULL, NULL);
		if (winapires == 0)
		{
			throw std::runtime_error("Error call WideCharToMultiByte");
		}


		return std::string(temp);
	}
	return "";
}
PWSTR SaveFile_Dialoge()
{
	PWSTR pszFilePath = PWSTR();
	bool success = false;
	COMDLG_FILTERSPEC FileTypes[] = {
		{ L"ASCII Ships data", L"*.asdat" },
		{ L"All files", L"*.*" }
	};
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileSaveDialog* pFileOpen;
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			pFileOpen->SetFileTypes(_countof(FileTypes), FileTypes);
			hr = pFileOpen->Show(NULL);
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						success = true;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return success ? pszFilePath : NULL;
}
PWSTR OpenFile_Dialoge()
{
	PWSTR pszFilePath = PWSTR();
	bool success = false;
	COMDLG_FILTERSPEC FileTypes[] = {
		{ L"ASCII Ships data", L"*.asdat" },
		{ L"All files", L"*.*" }
	};
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		if (SUCCEEDED(hr))
		{
			pFileOpen->SetFileTypes(_countof(FileTypes), FileTypes);
			hr = pFileOpen->Show(NULL);
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						success = true;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return success ? pszFilePath : NULL;
}

#define Is_UpKey(key_code) key_code == 72 || key_code == 119 || key_code == 56 || key_code == -26
#define Is_LeftKey(key_code) key_code == 75 || key_code == 97 || key_code == 52 || key_code == -28
#define Is_DownKey(key_code) key_code == 80 || key_code == 115 || key_code == 50 || key_code == -21
#define Is_RightKey(key_code) key_code == 77 || key_code == 100 || key_code == 54 || key_code == -94
#define Is_AcceptKey(key_code) key_code == 13 || key_code == 32 || key_code == 53
#define Is_CancelKey(key_code) key_code == 27
#define Is_DeleteKey(key_code) key_code == 83 ||  key_code == 120 ||  key_code == -25
#define Is_CopyKey(key_code) key_code == 3
#define Is_SaveKey(key_code) key_code == 19
#define Is_LoadKey(key_code) key_code == 15
#define Is_ExitKey(key_code) key_code == 17
#define Is_HelpKey(key_code) key_code == 60 || key_code == 8 ||  key_code == 9

int main()
{
	setlocale(LC_ALL, "rus");
	Window_Args args{ cons };
	Point2 debug_point = Point2(0, args.Size.Y + 1);
	PWSTR file_path = PWSTR();

	cout << "�������: 'F2' Ctrl + h, Ctrl + i" << endl;
	for (int y = 0; y < args.Size.Y; y++)
	{
		for (int x = 0; x < args.Size.X; x++)
		{
			if (y == 0 || y == args.Size.Y - 1)
				cout << '-';
			else if (x == 0 || x == args.Size.X - 1)
				cout << '|';
			else
				cout << ' ';
		}
		cout << "\n";
	}

	while (true)
	{
		if (args.update)
		{
			Update_Window(args);
			SetConsoleWindowInfo(cons, TRUE, args.Size);
		}
		char c = _getch();
		if (c == -32)
			c = _getch();

		if (Is_UpKey(c))
		{
			args.On_Key_UP();
		}
		else if (Is_LeftKey(c))
		{
			args.On_Key_Left();
		}
		else if (Is_DownKey(c))
		{
			args.On_Key_Down();
		}
		else if (Is_RightKey(c))
		{
			args.On_Key_Right();
		}
		else if (Is_AcceptKey(c))
		{
			args.On_Key_Accept();
		}
		else if (Is_CancelKey(c))
		{
			args.On_Key_Cancel();
		}
		else if (Is_DeleteKey(c))
		{
			args.On_Key_Delete();
		}
		else if (Is_CopyKey(c))
		{
			if (args.parts_count == 0)
				MessageBoxW(NULL, L"������ ���������", L"��������������", MB_ICONEXCLAMATION);
			else if (OpenClipboard(NULL))
			{
				// Remove the current Clipboard contents
				if (EmptyClipboard())
				{
					// Get the currently selected data
					Part* top_part = NULL;
					Part* left_part = NULL;
					Part* right_part = NULL;
					Part* bottom_part = NULL;
					for (int i = 0; i < args.parts_count; i++)
					{
						Part* p = &args.parts[i];
						if (top_part == NULL || top_part->Position.Y + top_part->Center.Y > p->Position.Y + p->Center.Y)
							top_part = p;
						if (bottom_part == NULL || bottom_part->Position.Y + bottom_part->Center.Y - bottom_part->Size.Y < p->Position.Y + p->Center.Y - p->Size.Y)
							bottom_part = p;

						if (left_part == NULL || left_part->Position.X - left_part->Center.X > p->Position.X - p->Center.X)
							left_part = p;
						if (right_part == NULL || right_part->Position.X + right_part->Center.X < p->Position.X + p->Center.X)
							right_part = p;
					}

					Point2 size = Point2(
						(right_part->Position.X - right_part->Center.X + right_part->Size.X-1) - (left_part->Position.X - left_part->Center.X),
						(bottom_part->Position.Y - bottom_part->Center.Y + bottom_part->Size.Y) - (top_part->Position.Y - top_part->Center.Y)
					);
					int len = (size.X + 1) * size.Y;
					HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, len);

					char* data = new char[len];
					for (int y = 0; y < size.Y; y++)
					{
						for (int x = 0; x < size.X; x++)
						{
							int index = y * (size.X+1) + x;
							data[index] = ' ';
						}
						data[(y + 1) * (size.X+1)-1] = '\n';
					}
					data[len - 1] = '\0';

					Point2 start = Point2(left_part->Position.X - left_part->Center.X, top_part->Position.Y - top_part->Center.Y);
					for (int i = 0; i < args.parts_count; i++)
					{
						Part* p = &args.parts[i];
						Point2 d_pos = p->Position - start - p->Center;
						for (int y = 0; y < p->Size.Y; y++)
						{
							for (int x = 0; x < p->Size.X-1; x++)
							{
								int index = (d_pos.X + x) + (size.X + 1) * (d_pos.Y + y);
								data[index] = p->Sprite[y][x];
							}
						}
					}
					strcpy_s((char*)hGlob, len, data);
					// For the appropriate data formats...
					if (SetClipboardData(CF_TEXT, hGlob) == NULL)
					{
						CloseClipboard();
						GlobalFree(hGlob);
					}
					else
					{
						CloseClipboard();
						MessageBoxW(NULL, L"����������� � ������ ������", L"����������", MB_ICONINFORMATION);
					}
				}
			}
			
		}
		else if (Is_SaveKey(c))
		{
			if (args.parts_count == 0)
			{
				MessageBoxW(NULL, L"��������� ������!", L"��������������", MB_ICONEXCLAMATION);
			}
			else
			{
				file_path = SaveFile_Dialoge();
				if (file_path == NULL)
					MessageBoxW(NULL, L"�� �� ������� ����!", L"��������������", MB_ICONEXCLAMATION);
				else
				{
					FILE* f = NULL;
					fopen_s(&f, convertWstringToAstringWinapi(wstring(file_path)).c_str(), "w");
					fwrite(&args.parts_count, sizeof(int), 1, f);
					for (int i = 0; i < args.parts_count; i++)
					{
						Part* p = &args.parts[i];
						fwrite(&p->Type, sizeof(Part_Type), 1, f);
						fwrite(&p->Position, sizeof(Point2), 1, f);
					}
					int NULL_key = -1;
					for (int i = 0; i < args.parts_count; i++)
					{
						Part* p = &args.parts[i];
						for (int ic = 0; ic < p->Connectors_Count; ic++)
						{
							PartConnector* c = &p->Connectors[ic];
							if (c->Connected == NULL)
								fwrite(&NULL_key, sizeof(int), 1, f);
							else
							{
								fwrite(&c->Connected->Main->Index, sizeof(int), 1, f);
								fwrite(&c->Connected->Index, sizeof(int), 1, f);
							}
						}
					}
					fclose(f);
					MessageBoxW(NULL, L"������� ���������.", L"����������", MB_ICONINFORMATION);
				}
			}
		}
		else if (Is_LoadKey(c))
		{
			file_path = OpenFile_Dialoge();
			if (file_path == NULL)
				MessageBoxW(NULL, L"�� �� ������� ����!", L"��������������", MB_ICONEXCLAMATION);
			else
			{
				FILE* f = NULL;
				fopen_s(&f, convertWstringToAstringWinapi(wstring(file_path)).c_str(), "r");
				fread(&args.parts_count, sizeof(int), 1, f);
				args.parts = new Part[args.parts_count];
				for (int i = 0; i < args.parts_count; i++)
				{
					Part_Type t;
					fread(&t, sizeof(Part_Type), 1, f);
					Part* p = Get_Part(t);
					fread(&p->Position, sizeof(Point2), 1, f);
					p->Index = i;
					args.parts[i] = *p;
				}
				for (int i = 0; i < args.parts_count; i++)
				{
					for (int ic = 0; ic < args.parts[i].Connectors_Count; ic++)
					{
						PartConnector* c = &args.parts[i].Connectors[ic];
						int index;
						fread(&index, sizeof(int), 1, f);
						if (index != -1)
						{
							int pci;
							fread(&pci, sizeof(int), 1, f);
							c->Connected = &args.parts[index].Connectors[pci];
						}
					}
				}
				args.connector_selecting = true;
				args.part_selecting = false;
				args.Reset();
				fclose(f);
				MessageBoxW(NULL, L"�������� ������ ������ �������.", L"����������", MB_ICONINFORMATION);
			}
		}
		else if (Is_HelpKey(c))
			MessageBoxW(NULL, L"����� ���������� � ��������.\n����������:\n WASD, Numpad, ��������� - ���������\n Space, Enter, Num0 - ��������� ��������\n Delete - ������� ������ � �������\n Esc - ��������/�� ������� ������\n Ctrl + c - ����������� �������� � ����� ������\n Ctrl + s - ��������� ������ � ����\n Ctrl + o - ��������� ������ �� �����", L"���������� ����������", MB_ICONQUESTION);
		else if (Is_ExitKey(c))
			return 0;
	}
}