
// RecursiveLogicDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RecursiveLogic.h"
#include "RecursiveLogicDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include <iostream>
#include <vector>
#include <string>
#include <map>
using namespace std;

vector<string> cname;//������
vector<string> vname;//������
vector<string> fname;//������

					 //���������
enum identifier_type { variable, constant };
typedef struct {
	identifier_type type;//���ֳ��������
	int id;//���
}identifier;

//ԭ��ʽ
typedef struct {
	vector<identifier> element;//ԭ��ʽ�а����ĳ����ͱ���
	int function_id;//�������
	bool ispos;//�ж��Ƿ���ȡ������
}atom;

//�Ӿ�
typedef struct {
	vector<atom> element;//�Ӿ��а�����ԭ�Ӿ䣬������Ϊ1��˵��Ϊ��ʵ�Ӿ䣬�ɶ������Ӿ�����滻
	int parent_1;//��Դ1��һ��Ϊ���滻�ĸ����Ӿ䣬û������Ϊ-1
	int parent_2;//��Դ2��һ��Ϊ�����滻����ʵ�Ӿ䣬û������Ϊ-1
	bool isvalid;//�ж���ʵ�Ӿ��Ƿ��ܽ����滻
}clause;

vector<clause> clause_set;//�Ӿ伯
clause target_clause;//Ŀ���Ӿ�




//��麯���Ƿ��Ѿ�������vector��
bool find_function(string name, int& id)
{
	vector<string>::iterator func = find(fname.begin(), fname.end(), name);
	if (func == fname.end())
		return false; //δ�ҵ�
	else
	{
		id = func - fname.begin();
		return true;  //�ҵ���ȡ����id
	}
}

//��鳣��������Ƿ��Ѿ�������vector��
bool find_identifier(string name, identifier& it)
{
	//����
	vector<string>::iterator iden = find(cname.begin(), cname.end(), name);
	if (iden != cname.end())
	{
		it.id = iden - cname.begin();
		it.type = constant;
		return true;
	}
	//����
	iden = find(vname.begin(), vname.end(), name);
	if (iden != vname.end())
	{
		it.id = iden - vname.begin();
		it.type = variable;
		return true;
	}

	//MessageBox( "��������Ӿ��к���δ�涨�ı���/��������" );
	return false;
}

//����һ��ԭ�Ӿ�
bool input_atom(clause& pc, string& clause_input, int atom_start, int atom_end)
{
	int identifier_start, identifier_end, i;
	atom presnet_atom;
	//�ж��Ƿ���ȡ������
	if (clause_input[atom_start] == '~')
	{
		presnet_atom.ispos = false;
		atom_start++;
	}
	//ȡ��������
	string present_fname;
	for (i = atom_start; i < atom_end; i++)
		if (clause_input[i] == '(')
		{
			present_fname = clause_input.substr(atom_start, i - atom_start);
			identifier_start = i + 1;
			break;
		}

	if (i == atom_end)
	{
		cout << "���벻�����Ӿ��ʽ��" << endl;
		return false;
	}
	//ȡ����������vector�е�id
	int id;
	bool find = find_function(present_fname, id);
	if (!find)
	{
		fname.push_back(present_fname);
		presnet_atom.function_id = fname.size() - 1;  //δ�ҵ������
	}
	else
		presnet_atom.function_id = id;  //�ҵ���ȡ����id

										//���볣�������
	for (i = identifier_start; i < atom_end; i++)
		if ((clause_input[i] == ',') || (clause_input[i] == ')'))
		{
			identifier_end = i;
			identifier it;
			//���������������ԭ�Ӿ��element��
			if (!find_identifier(clause_input.substr(identifier_start, identifier_end - identifier_start), it))
				return false;
			presnet_atom.element.push_back(it);
			identifier_start = i + 1;
		}

	pc.element.push_back(presnet_atom);
	return true;
}

//�����Ӿ�
bool input_clause()
{
	cout << "�����������Ӿ伯( | Ϊ�� ~ Ϊ��)��������һ�е�#��β��" << endl;
	cout << "��������ʹ�ñ���Ϊx,y,z,����ΪA,B,C" << endl;
	cout << "������Ϊ�ư������Ӿ伯����ֱ�Ӹ���ʹ�ã�" << endl;
	cout << "K(C,A)|K(B,A)|K(A,A)" << endl;
	cout << "H(x,A)|~K(x,A)" << endl;
	cout << "~H(C,x)|~H(A,x)" << endl;
	cout << "H(A,A)" << endl;
	cout << "H(A,C)" << endl;
	cout << "R(x,A)|H(B,x)" << endl;
	cout << "~H(A,x)|H(B,x)" << endl;
	cout << "~H(A,A)|~H(A,B)|~H(A,C)" << endl;
	cout << "~H(B,A)|~H(B,B)|~H(B,C)" << endl;
	cout << "~H(C,A)|~H(C,B)|~H(C,C)" << endl;
	cout << "~R(x,A)|~K(x,A)" << endl;
	cout << "#" << endl;
	while (1)
	{
		clause pc;
		pc.parent_1 = -1;
		pc.parent_2 = -1;
		pc.isvalid = true;

		string clause_input;
		cin >> clause_input;
		if (clause_input == "#")
			break;

		unsigned int i, atom_start = 0, atom_end = 0;
		for (i = 0; i < clause_input.size(); i++)
			if ((clause_input[i] == '|') || (i == clause_input.size() - 1))
			{
				atom_end = (i == clause_input.size() - 1) ? i + 1 : i;
				if (!input_atom(pc, clause_input, atom_start, atom_end))
					return false;
				atom_start = i + 1;
			}

		clause_set.push_back(pc);
	}
	return true;
}

//����Ŀ���Ӿ�
bool input_target(string str)
{
	cout << endl;
	cout << "������Ŀ���Ӿ䣨��K(A,A)����" << endl;
	target_clause.parent_1 = -1;
	target_clause.parent_2 = -1;
	target_clause.isvalid = true;

	string clause_input;
	//cin >> clause_input;
	//(CW2A(m_target)) >> clause_input;
	clause_input = str;
	unsigned int i, atom_start = 0, atom_end = 0;
	for (i = 0; i < clause_input.size(); i++)
		if ((clause_input[i] == '|') || (i == clause_input.size() - 1))
		{
			atom_end = (i == clause_input.size() - 1) ? i + 1 : i;
			if (!input_atom(target_clause, clause_input, atom_start, atom_end))
				return false;
			atom_start = i + 1;
		}

	return true;
}



//���������
bool operator<(identifier a, identifier b)
{
	return a.id > b.id;
}

//��������أ���������Ӿ�
string out( clause& c)
{
	string out="";
	for (unsigned int i = 0; i < c.element.size(); i++)
	{
		if (!c.element[i].ispos)
			out += "~";
		out += fname[c.element[i].function_id] + "(";
		for (unsigned int j = 0; j < c.element[i].element.size(); j++)
		{
			if (c.element[i].element[j].type == variable)
				out += vname[c.element[i].element[j].id];
			else
				out += cname[c.element[i].element[j].id];
			if (j != c.element[i].element.size() - 1)
				out += ",";
		}
		out += ")";
		if (i != c.element.size() - 1)
			out += "|";
	}

	return out;
}

//�жϺ��������Ƿ�һ��
bool isequal(atom& atom1, atom& atom2)
{
	if ((atom1.function_id == atom2.function_id) && (atom1.element.size() == atom2.element.size()))
	{
		for (unsigned int i = 0; i < atom1.element.size() && i < atom2.element.size(); i++)
		{
			if ((atom1.element[i].type == variable) || (atom2.element[i].type == variable))
				continue;
			if (atom1.element[i].id != atom2.element[i].id)
				return false;
		}
		return true;
	}
	return false;
}



//�ж�a�Ƿ���b�ĺ��ӽ��
bool is_child(int a, int b)
{
	if ((clause_set[a].parent_1 == -1) || (clause_set[a].parent_2 == -1))
		return false;
	if ((clause_set[a].parent_1 == b) || (clause_set[a].parent_2 == b))
		return true;
	return(is_child(clause_set[a].parent_1, b) || is_child(clause_set[a].parent_2, b));
}

//ͨ�����������Ӿ䣬����parent_1Ϊ���滻���Ӿ䣬parent_2Ϊ��ʵ�Ӿ䣬todelete�Ǵ�ɾ����ԭ�Ӿ�
int create_new_clause(int parent_2, int parent_1, int todelete)
{
	clause new_clause;
	map<identifier, identifier> replace_map;//ʵ���������滻�ļ�ֵ�ԣ�ǰΪ��������Ϊ����
	for (unsigned int i = 0; i < clause_set[parent_2].element[0].element.size(); i++)
		if (clause_set[parent_1].element[todelete].element[i].type == variable)
			replace_map[clause_set[parent_1].element[todelete].element[i]] = clause_set[parent_2].element[0].element[i];

	new_clause.element = clause_set[parent_1].element;
	new_clause.parent_1 = parent_1;
	new_clause.parent_2 = parent_2;
	new_clause.isvalid = true;
	new_clause.element.erase(vector<atom>::iterator(new_clause.element.begin() + todelete));

	for (unsigned int i = 0; i < new_clause.element.size(); i++)
		for (unsigned int j = 0; j < new_clause.element[i].element.size(); j++)
			if ((new_clause.element[i].element[j].type == variable) &&
				(replace_map.end() != replace_map.find(new_clause.element[i].element[j])))
				new_clause.element[i].element[j] = replace_map[new_clause.element[i].element[j]];

	if (new_clause.element.size() == 0)
		return -1;  //�������Ӿ䣬��ԭ�Ӿ�����߼�ì�ܣ�����-1

	clause_set.push_back(new_clause);   //�����Ӿ���뵽�Ӿ伯��

	if (new_clause.element.size() == target_clause.element.size() &&
		isequal(new_clause.element[0], target_clause.element[0]) &&
		new_clause.element[0].ispos == target_clause.element[0].ispos)
		return 1;  //�������Ӿ���Ŀ���Ӿ���ȫһ�£����ɹ�������1

	return 0;
}

//Ѱ�ҿ��Ա�������������Ӿ���Ӿ䣬��parent_1������numΪ��ʵ�Ӿ��λ��
int replace(int num)
{
	for (unsigned int i = 0; i < clause_set.size(); i++)
		if (clause_set[i].isvalid && !is_child(num, i))
		{
			for (unsigned int j = 0; j < clause_set[i].element.size(); j++)
				if (isequal(clause_set[num].element[0], clause_set[i].element[j]) &&
					clause_set[num].element[0].ispos != clause_set[i].element[j].ispos) //�����Ӿ�����෴���ɵ���
				{
					int state = create_new_clause(num, i, j);
					if (state == -1 || state == 1)
						return state;
				}
		}

	return 0;
}

//��ắ��,Ѱ����ʵ�Ӿ䣬��parent_2
int resolution()
{
	for (unsigned int i = 0; i < clause_set.size(); i++)
		if (clause_set[i].element.size() == 1) //�Ӿ���ֻ��һ��ԭ�Ӿ䣬˵�����Ӿ�Ϊ��ʵ�Ӿ�
		{
			clause_set[i].isvalid = false; //˵���ѱ�ʹ��
			int state = replace(i);
			if (state == -1 || state == 1)
				return state;
		}

	return 0;
}

//��ӡ������
string print(clause& pc)
{
	if (pc.parent_2 == -1 && pc.parent_1 == -1)
		return "";
	string s1 = print(clause_set[pc.parent_2]);
	string s2 = print(clause_set[pc.parent_1]);

	return s1 + s2 + out(clause_set[pc.parent_2]) + "  &  " + out(clause_set[pc.parent_1]) + "  =>  " + out(pc) + "\r\n";
}


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRecursiveLogicDlg �Ի���



CRecursiveLogicDlg::CRecursiveLogicDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RECURSIVELOGIC_DIALOG, pParent)
	, m_subs(_T(""))
	, m_target(_T(""))
	, m_subsout(_T(""))
	, m_targetout(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	string x = "x";
	string y = "y";
	string z = "z";
	string A = "A";
	string B = "B";
	string C = "C";
	vname.push_back(x);
	vname.push_back(y);
	vname.push_back(z);
	cname.push_back(A);
	cname.push_back(B);
	cname.push_back(C);
	m_inputfinished = false;
	m_needclear = false;


	m_subsout += "�����������Ӿ伯( | Ϊ�� ~ Ϊ��)��������һ�е�#��β��\r\n";
	m_subsout += "��������ʹ�ñ���Ϊx,y,z,����ΪA,B,C\r\n";
	m_subsout +=  "������Ϊ�ư������Ӿ伯����ֱ�Ӹ���ʹ�ã�\r\n";
	m_subsout += "K(C,A)|K(B,A)|K(A,A)\r\n";
	m_subsout += "H(x,A)|~K(x,A)\r\n";
	m_subsout += "~H(C,x)|~H(A,x)\r\n";
	m_subsout += "H(A,A)\r\n";
	m_subsout += "H(A,C)\r\n";
	m_subsout +=  "R(x,A)|H(B,x)\r\n";
	m_subsout += "~H(A,x)|H(B,x)\r\n";
	m_subsout += "~H(A,A)|~H(A,B)|~H(A,C)\r\n";
	m_subsout += "~H(B,A)|~H(B,B)|~H(B,C)\r\n";
	m_subsout += "~H(C,A)|~H(C,B)|~H(C,C)\r\n";
	m_subsout += "~R(x,A)|~K(x,A)\r\n";
	m_subsout += "#\r\n";

	m_subsout = "";
}

void CRecursiveLogicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITSUBS, m_subs);
	DDX_Text(pDX, IDC_EDITTARGET, m_target);
	DDX_Text(pDX, IDC_SUBSOUT, m_subsout);
	DDX_Text(pDX, IDC_TARGETOUT, m_targetout);
}

BEGIN_MESSAGE_MAP(CRecursiveLogicDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SUBS, &CRecursiveLogicDlg::OnBnClickedSubs)
	ON_BN_CLICKED(IDC_TARGET, &CRecursiveLogicDlg::OnBnClickedTarget)
	ON_BN_CLICKED(IDC_FINISH, &CRecursiveLogicDlg::OnBnClickedFinish)
	ON_BN_CLICKED(IDC_CLEAR, &CRecursiveLogicDlg::OnBnClickedClear)
END_MESSAGE_MAP()


// CRecursiveLogicDlg ��Ϣ�������

BOOL CRecursiveLogicDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CRecursiveLogicDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRecursiveLogicDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRecursiveLogicDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRecursiveLogicDlg::OnBnClickedSubs()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	UpdateData(true);

	char buf[512] = { 0 };
	string clause_input = (const char *)m_subs;// .GetBuffer(m_subs.GetLength());

	clause pc;
	pc.parent_1 = -1;
	pc.parent_2 = -1;
	pc.isvalid = true;

	if (m_needclear)
	{
		m_subsout = "";
		m_needclear = false;
		clause_set.clear();
	}

	if (m_inputfinished)
	{
		if (IDYES == MessageBox("ȷ��Ҫ����������������Ӿ䣬���¿�ʼ������", "���¿�ʼ����", MB_YESNO))
		{
			m_inputfinished = false;
			clause_set.clear();
			m_subsout = "";
		}
		else
			return;
	}
	
	int start = 0;
	do
	{
		unsigned int i, atom_start = 0, atom_end = 0;
		for (i = 0; i < clause_input.size(); i++)
			if ((clause_input[i] == '|') || (i == clause_input.size() - 1))
			{
				atom_end = (i == clause_input.size() - 1) ? i + 1 : i;
				if (!input_atom(pc, clause_input, atom_start, atom_end))
				{
					MessageBox("�Ӿ��������");
					return;
				}
				atom_start = i + 1;
			}

		clause_set.push_back(pc);
	} while (0);// m_subs.Find("\r\n", start)>0);

	m_subsout += m_subs + "\r\n";
	m_subs = "";
	UpdateData(false);
}


void CRecursiveLogicDlg::OnBnClickedTarget()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);

	if (m_inputfinished == false)
	{
		MessageBox("���������Ӿ伯��");
		return;
	}

	string s=m_target.GetBuffer(0);
	if (!input_target(s))
		MessageBox("Ŀ���Ӿ��������");

	m_targetout = "\r\n";

	int state = resolution();
	m_targetout += print(clause_set.back()).c_str();

	if (state == 1)
		m_targetout += "���ɹ���";
	else if (state == 0)
		m_targetout += "���ʧ�ܣ�";
	else if (state == -1)
		m_targetout += "�Ӿ��߼����ִ���!";

	m_inputfinished = false;
	m_needclear = true;
	UpdateData(false);
}


void CRecursiveLogicDlg::OnBnClickedFinish()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (1)
	{
		m_inputfinished = true;
		MessageBox("��������Ŀ���Ӿ���в����ˣ�");
		return;
	}
}


void CRecursiveLogicDlg::OnBnClickedClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_subsout = "";
	UpdateData(false);
	clause_set.clear();
}
