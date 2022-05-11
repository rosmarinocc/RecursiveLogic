
// RecursiveLogicDlg.cpp : 实现文件
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

vector<string> cname;//常量名
vector<string> vname;//变量名
vector<string> fname;//函数名

					 //常量与变量
enum identifier_type { variable, constant };
typedef struct {
	identifier_type type;//区分常量或变量
	int id;//编号
}identifier;

//原子式
typedef struct {
	vector<identifier> element;//原子式中包含的常量和变量
	int function_id;//函数编号
	bool ispos;//判断是否有取反符号
}atom;

//子句
typedef struct {
	vector<atom> element;//子句中包含的原子句，若个数为1，说明为事实子句，可对其他子句进行替换
	int parent_1;//来源1，一般为被替换的复合子句，没有则置为-1
	int parent_2;//来源2，一般为进行替换的事实子句，没有则置为-1
	bool isvalid;//判断事实子句是否还能进行替换
}clause;

vector<clause> clause_set;//子句集
clause target_clause;//目标子句




//检查函数是否已经储存在vector中
bool find_function(string name, int& id)
{
	vector<string>::iterator func = find(fname.begin(), fname.end(), name);
	if (func == fname.end())
		return false; //未找到
	else
	{
		id = func - fname.begin();
		return true;  //找到并取出其id
	}
}

//检查常量或变量是否已经储存在vector中
bool find_identifier(string name, identifier& it)
{
	//常量
	vector<string>::iterator iden = find(cname.begin(), cname.end(), name);
	if (iden != cname.end())
	{
		it.id = iden - cname.begin();
		it.type = constant;
		return true;
	}
	//变量
	iden = find(vname.begin(), vname.end(), name);
	if (iden != vname.end())
	{
		it.id = iden - vname.begin();
		it.type = variable;
		return true;
	}

	//MessageBox( "输入错误，子句中含有未规定的变量/常量名！" );
	return false;
}

//读入一个原子句
bool input_atom(clause& pc, string& clause_input, int atom_start, int atom_end)
{
	int identifier_start, identifier_end, i;
	atom presnet_atom;
	//判断是否有取反符号
	if (clause_input[atom_start] == '~')
	{
		presnet_atom.ispos = false;
		atom_start++;
	}
	//取出函数名
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
		cout << "输入不符合子句格式！" << endl;
		return false;
	}
	//取出函数名在vector中的id
	int id;
	bool find = find_function(present_fname, id);
	if (!find)
	{
		fname.push_back(present_fname);
		presnet_atom.function_id = fname.size() - 1;  //未找到则存入
	}
	else
		presnet_atom.function_id = id;  //找到则取出其id

										//分离常量或变量
	for (i = identifier_start; i < atom_end; i++)
		if ((clause_input[i] == ',') || (clause_input[i] == ')'))
		{
			identifier_end = i;
			identifier it;
			//将常量或变量存入原子句的element中
			if (!find_identifier(clause_input.substr(identifier_start, identifier_end - identifier_start), it))
				return false;
			presnet_atom.element.push_back(it);
			identifier_start = i + 1;
		}

	pc.element.push_back(presnet_atom);
	return true;
}

//输入子句
bool input_clause()
{
	cout << "请逐行输入子句集( | 为或， ~ 为非)，以另起一行的#结尾：" << endl;
	cout << "声明：可使用变量为x,y,z,常量为A,B,C" << endl;
	cout << "（以下为破案问题子句集，可直接复制使用）" << endl;
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

//输入目标子句
bool input_target(string str)
{
	cout << endl;
	cout << "请输入目标子句（如K(A,A)）：" << endl;
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



//运算符重载
bool operator<(identifier a, identifier b)
{
	return a.id > b.id;
}

//输出流重载，便于输出子句
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

//判断函数类型是否一致
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



//判断a是否是b的孩子结点
bool is_child(int a, int b)
{
	if ((clause_set[a].parent_1 == -1) || (clause_set[a].parent_2 == -1))
		return false;
	if ((clause_set[a].parent_1 == b) || (clause_set[a].parent_2 == b))
		return true;
	return(is_child(clause_set[a].parent_1, b) || is_child(clause_set[a].parent_2, b));
}

//通过归结产生新子句，其中parent_1为被替换的子句，parent_2为事实子句，todelete是待删除的原子句
int create_new_clause(int parent_2, int parent_1, int todelete)
{
	clause new_clause;
	map<identifier, identifier> replace_map;//实例化进行替换的键值对，前为变量，后为常量
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
		return -1;  //归结出空子句，即原子句存在逻辑矛盾，返回-1

	clause_set.push_back(new_clause);   //将新子句加入到子句集中

	if (new_clause.element.size() == target_clause.element.size() &&
		isequal(new_clause.element[0], target_clause.element[0]) &&
		new_clause.element[0].ispos == target_clause.element[0].ispos)
		return 1;  //归结出的子句与目标子句完全一致，归结成功，返回1

	return 0;
}

//寻找可以被替代并产生新子句的子句，即parent_1，其中num为事实子句的位置
int replace(int num)
{
	for (unsigned int i = 0; i < clause_set.size(); i++)
		if (clause_set[i].isvalid && !is_child(num, i))
		{
			for (unsigned int j = 0; j < clause_set[i].element.size(); j++)
				if (isequal(clause_set[num].element[0], clause_set[i].element[j]) &&
					clause_set[num].element[0].ispos != clause_set[i].element[j].ispos) //两个子句符号相反，可抵消
				{
					int state = create_new_clause(num, i, j);
					if (state == -1 || state == 1)
						return state;
				}
		}

	return 0;
}

//归结函数,寻找事实子句，即parent_2
int resolution()
{
	for (unsigned int i = 0; i < clause_set.size(); i++)
		if (clause_set[i].element.size() == 1) //子句中只有一个原子句，说明该子句为事实子句
		{
			clause_set[i].isvalid = false; //说明已被使用
			int state = replace(i);
			if (state == -1 || state == 1)
				return state;
		}

	return 0;
}

//打印归结过程
string print(clause& pc)
{
	if (pc.parent_2 == -1 && pc.parent_1 == -1)
		return "";
	string s1 = print(clause_set[pc.parent_2]);
	string s2 = print(clause_set[pc.parent_1]);

	return s1 + s2 + out(clause_set[pc.parent_2]) + "  &  " + out(clause_set[pc.parent_1]) + "  =>  " + out(pc) + "\r\n";
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CRecursiveLogicDlg 对话框



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


	m_subsout += "请逐行输入子句集( | 为或， ~ 为非)，以另起一行的#结尾：\r\n";
	m_subsout += "声明：可使用变量为x,y,z,常量为A,B,C\r\n";
	m_subsout +=  "（以下为破案问题子句集，可直接复制使用）\r\n";
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


// CRecursiveLogicDlg 消息处理程序

BOOL CRecursiveLogicDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRecursiveLogicDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRecursiveLogicDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRecursiveLogicDlg::OnBnClickedSubs()
{
	// TODO: 在此添加控件通知处理程序代码

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
		if (IDYES == MessageBox("确定要放弃所有已输入的子句，重新开始输入吗？", "重新开始输入", MB_YESNO))
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
					MessageBox("子句输入错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	if (m_inputfinished == false)
	{
		MessageBox("请先输入子句集！");
		return;
	}

	string s=m_target.GetBuffer(0);
	if (!input_target(s))
		MessageBox("目标子句输入错误！");

	m_targetout = "\r\n";

	int state = resolution();
	m_targetout += print(clause_set.back()).c_str();

	if (state == 1)
		m_targetout += "归结成功！";
	else if (state == 0)
		m_targetout += "归结失败！";
	else if (state == -1)
		m_targetout += "子句逻辑出现错误!";

	m_inputfinished = false;
	m_needclear = true;
	UpdateData(false);
}


void CRecursiveLogicDlg::OnBnClickedFinish()
{
	// TODO: 在此添加控件通知处理程序代码
	if (1)
	{
		m_inputfinished = true;
		MessageBox("可以输入目标子句进行测试了！");
		return;
	}
}


void CRecursiveLogicDlg::OnBnClickedClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_subsout = "";
	UpdateData(false);
	clause_set.clear();
}
