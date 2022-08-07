#pragma once
#ifndef MYFRAMEWORK_SELECTION_H
#define MYFRAMEWORK_SELECTION_H

#if defined BUILD_WITH_VCL
#include <System.Classes.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ComCtrls.hpp>

struct is_delpi_compatible : std::true_type {};
struct is_vcl_compile : std::true_type {};
struct is_fmx_compile : std::false_type {};
struct is_qt_compile : std::false_type {};
struct is_nuk_compile : std::false_type {};

using TForm = Vcl::Forms::TForm;
using TGroupBox = Vcl::Stdctrls::TGroupBox;
using TEdit = Vcl::Stdctrls::TEdit;
using TLabel = Vcl::Stdctrls::TLabel;
using TButton = Vcl::Stdctrls::TButton;
using TCheckBox = Vcl::Stdctrls::TCheckBox;
using TListBox = Vcl::Stdctrls::TListBox;
using TComboBox = Vcl::Stdctrls::TComboBox;
using TMemo = Vcl::Stdctrls::TMemo;
using TStatusBar = Vcl::Comctrls::TStatusBar;
using TListView = Vcl::Comctrls::TListView;
using TPageControl = Vcl::Comctrls::TPageControl;

using fw_String = UnicodeString;
using fw_Form = TForm;
using fw_Groupbox = TGroupBox;
using fw_Edit = TEdit;
using fw_Memo = TMemo;
using fw_Label = TLabel;
using fw_Combobox = TComboBox;
using fw_Listbox = TListBox;
using fw_Checkbox = TCheckBox;
using fw_Button = TButton;
using fw_Table = TListView;
using fw_Statusbar = TStatusBar;

#elif defined BUILD_WITH_FMX
#include <System.Classes.hpp>
#include <Fmx.Forms.hpp>
#include <Fmx.StdCtrls.hpp>
#include <Fmx.Edit.hpp>
#include <Fmx.ListBox.hpp>
#include <Fmx.Grid.hpp>
#include <Fmx.Grid.Style.hpp>
#include <Fmx.TabControl.hpp>

struct is_delpi_compatible : std::true_type {};
struct is_vcl_compile : std::false_type {};
struct is_fmx_compile : std::true_type {};
struct is_qt_compile : std::false_type {};
struct is_nuk_compile : std::false_type {};

using TForm = Fmx::Forms::TForm;
using TGroupBox = Fmx::Stdctrls::TGroupBox;
using TEdit = Fmx::Edit::TEdit;
using TLabel = Fmx::Stdctrls::TLabel;
using TButton = Fmx::Stdctrls::TButton;
using TCheckBox = Fmx::Stdctrls::TCheckBox;
using TComboBox = Fmx::Listbox::TComboBox;
using TListBox = Fmx::Listbox::TListBox;
using TComboBox = Fmx::Listbox::TComboBox;
using TMemo = Fmx::Memo::TMemo;
using TStringGrid = Fmx::Grid::TStringGrid;
using TTabControl = Fmx::Tabcontrol::TTabControl;

using fw_String = UnicodeString;
using fw_Form = TForm;
using fw_Groupbox = TGroupBox;
using fw_Edit = TEdit;
using fw_Memo = TMemo;
using fw_Label = TLabel;
using fw_Combobox = TComboBox;
using fw_Listbox = TListBox;
using fw_Checkbox = TCheckBox;
using fw_Button = TButton;
using fw_Table = TStringGrid;
using fw_Statusbar = TStatusBar;

#elif defined BUILD_WITH_QT
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QCheckBox>
#include <QTableWidget>
#include <QString>
#include <QStatusBar>

struct is_delpi_compatible : std::false_type {};
struct is_vcl_compile : std::false_type {};
struct is_fmx_compile : std::false_type {};
struct is_qt_compile : std::true_type {};
struct is_nuk_compile : std::false_type {};

using fw_String = QString;
using fw_Form = QWidget;
using fw_Groupbox = QGroupBox;
using fw_Edit = QLineEdit;
using fw_Memo = QTextEdit;
using fw_Label = QLabel;
using fw_Combobox = QComboBox;
using fw_Listbox = QListWidget;
using fw_Checkbox = QCheckBox;
using fw_Button = QPushButton;
using fw_Table = QTableWidget;
using fw_Statusbar = QStatusBar;

#elif defined BUILD_WITH_NUKLEAR
#include <vector>
#include <string>
#include <nuklear.h>

struct is_delpi_compatible : std::false_type {};
struct is_vcl_compile : std::false_type {};
struct is_fmx_compile : std::false_type {};
struct is_qt_compile : std::false_type {};
struct is_nuk_compile : std::true_type {};

using fw_String = std::string;
struct Undefined {};

struct TMemo
{
	std::string name;
	std::vector<std::string> data;
};

struct TLabel
{
	std::string name;
	std::string text;
};

struct TEdit
{
	std::string name;
	std::string text;
};

struct TCombobox
{
	std::string name;
	std::string text;
	std::vector<std::string> items;
};

struct TListbox
{
	std::string name;
	std::vector<std::string> items;
};

struct TCheckbox
{
	std::string name;
	std::string text;
	int checkstate = 0; //tristate?
};

struct TButton
{
	std::string name;
	std::string text;
	std::function<void()> onClick = nullptr;
};

struct TStatusBar
{
	std::string name;
	std::string text;
	int& window_height;
	int& window_width;
	int status_height;
	TStatusBar() = delete;
	TStatusBar(const TStatusBar&) = delete;
	TStatusBar(int& h, int& w)
		:
		window_height(h),
		window_width(w),
		status_height(30)
	{}
	void draw(nk_context* ctx)
	{
		if (nk_begin(ctx, name.c_str(), 
			nk_rect(0, window_height - status_height, 
					window_width, status_height),
			NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR
		))
		{
			nk_layout_row_dynamic(ctx, ctx->current->bounds.h, 1);
			nk_label(ctx, text.c_str(), NK_TEXT_CENTERED);
		}
		nk_end(ctx);
	}
};

struct TGroupBox
{
	std::string name;
	std::string title;
	std::vector<std::function<void(nk_context*)>> component_funcs;
	void draw(nk_context* ctx)
	{
		nk_group_begin_titled(ctx, name.c_str(), title.c_str(), nk_panel_flags::NK_WINDOW_BORDER);
		nk_layout_row_begin(ctx, nk_layout_format::NK_STATIC, 20, 1);
		for (auto& child : component_funcs)
		{
			child(ctx);
		}
		nk_layout_row_end(ctx);
		nk_group_end(ctx);
	}
};

using fw_Form = Undefined; //nk_begin..subcontrols..nk_end
using fw_Groupbox = TGroupBox; //nk_group + nk_option_labels?
using fw_Edit = TEdit; //nk_edit_string /NK_EDIT_FIELD
using fw_Label = TLabel; //nk_label(_colored/_wrap), nk_text
using fw_Memo = TMemo; //nk_layout_row(height..) + nk_edit_string /NK_EDIT_BOX
using fw_Combobox = TCombobox; //nk_combo
using fw_Listbox = TListbox; //nk_selectable_label + array_index
using fw_Checkbox = TCheckbox; //nk_checkbox_label
using fw_Button = TButton; //nk_button(_label/symbol/symbol_label)
using fw_Table = Undefined; //nk_layout_row_static(...,cols)
using fw_Statusbar = TStatusBar; //nk_begin(NO_TITLE, bounds=viewPort.width usw.) +nk_label

#else
static_assert(false, "Ein Framework muss gewählt sein, um diese Bibliothek zu nutzen"
#endif

#endif