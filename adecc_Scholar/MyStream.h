﻿#pragma once
#ifndef MyStreamH
#define MyStreamH

#include "MyStdTypes.h"
#include "MyFramework_Selection.h"


#if defined BUILD_WITH_VCL
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ComCtrls.hpp>

#elif defined BUILD_WITH_FMX
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Memo.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Grid.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.Types.hpp>

#elif defined BUILD_WITH_QT
#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>

#elif defined BUILD_WITH_NUKLEAR
//nuklear + nuk_controls, bereits da über MyFramework_Selection
#else
static_assert(false, " Für diese Bibliothek muss ein Framework definiert sein.
#endif

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>


	// ------------------------------------------------------------------------
	// Vorbereitung, Basisklassen sind C++, keine Frameworkabhängigkeit

	template <typename char_type, typename ty>
struct my_Ostream_Iter {
	using iterator_category = std::output_iterator_tag;
	using value_type = ty;
	using difference_type = void; // std::ptrdiff_t;
	using reference = void;
	using pointer = void;

	typename char_type::stream_type& out;
	TMyDelimiter<char_type> const& del;
	size_t pos;
	bool root;

	my_Ostream_Iter(std::ostream& s, TMyDelimiter<char_type> const& d) : out(s), del(d), pos(0u), root(true) {
		out << del.leading;
	}
	my_Ostream_Iter(my_Ostream_Iter<char_type, ty> const& ref) : out(ref.out), del(ref.del), pos(ref.pos), root(false) {}
	~my_Ostream_Iter() { if (root) out << del.trailing; }

	my_Ostream_Iter& operator=(my_Ostream_Iter<char_type, ty> const& p) {
		bool r = root;
		new (this) my_Ostream_Iter<char_type, ty>(p);
		root = r;
		return *this;
	}

	my_Ostream_Iter& operator=(ty const& e) {
		out << (pos > 0 ? del.center : char_type::strEmpty) << e;
		return *this;
	}

	my_Ostream_Iter& operator*() { return *this; }
	my_Ostream_Iter& operator++() {
		++pos;
		return *this;
	}

	my_Ostream_Iter& operator++(int) {
		pos++;
		return *this;
	}

};

template <typename ty>
using my_ostream_iterator = my_Ostream_Iter<Narrow, ty>;

template <typename ty>
using my_wostream_iterator = my_Ostream_Iter<Wide, ty>;


template <typename ty>
class StreamBufBase : public ty::streambuf {
protected:
	typename ty::stringstream os;
	using int_type = typename ty::streambuf::int_type;
public:
	StreamBufBase(void) { };
	virtual ~StreamBufBase(void) { };

	virtual void Write(void) = 0;

	int_type overflow(int_type c) {
		if (c == ty::cNL) {
			Write();
			os.str(ty::strEmpty);
		}
		else {
			os.put(c);
		}
		return c;
	}
};


template <typename ty>
class ListStreamBufBase : public StreamBufBase<ty> {
protected:
	using base_type = StreamBufBase<ty>;
	using int_type = typename ty::streambuf::int_type;
	std::vector<tplList<ty>> captions;
public:
	ListStreamBufBase(std::vector<tplList<ty>> const& para) : StreamBufBase<ty>() { captions = para; };
	virtual ~ListStreamBufBase(void) { };

	virtual void Write(void) = 0;
	virtual void NewLine(void) = 0;

	int_type overflow(int_type c) {
		switch (c) {
		case ty::cNL:
			Write();
			base_type::os.str(ty::strEmpty);
			NewLine();
			break;
		case ty::cTab:
			Write();
			base_type::os.str(ty::strEmpty);
			break;
		default:
			base_type::os.put(c);
		}
		return c;
	}
};


// ---------------------------------------------------------------------
// ab hier finden sich Framework- Abhängigkeiten
// Bezeichnung der Steuerelemente kann abweichen, Suchen von passenden Varianten
#if defined BUILD_WITH_VCL || defined BUILD_WITH_FMX
template <typename ty>
class MemoStreamBuf : public StreamBufBase<ty> {
private:
	TMemo* value;
public:
	MemoStreamBuf(TMemo* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;

		if (boClean)
			value->Lines->Clear();
	}

	virtual ~MemoStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (StreamBufBase<ty>::os.str().length() > 0) {
			value->Lines->Add(StreamBufBase<ty>::os.str().c_str());
		}
		else {
			value->Lines->Add(L"");
		}
	}
};

#elif defined BUILD_WITH_QT
template <typename ty>
class MemoStreamBuf : public StreamBufBase<ty> {
private:
	QTextEdit* value;
public:
	MemoStreamBuf(QTextEdit* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;

		if (boClean);
		value->clear();
	}

	virtual ~MemoStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (StreamBufBase<ty>::os.str().length() > 0) {
			if constexpr (std::is_same<Wide, ty>::value)
				value->append(QString::fromStdWString(StreamBufBase<ty>::os.str()));
			else if constexpr (std::is_same<Latin, ty>::value)
				value->append(QString::fromLatin1(QByteArray::fromStdString(StreamBufBase<ty>::os.str())));
			else
				value->append(QString::fromStdString(StreamBufBase<ty>::os.str()));
		}
		else {
			value->append("");
		}
	}
};

#elif defined BUILD_WITH_NUKLEAR
template <typename ty>
class MemoStreamBuf : public StreamBufBase<ty> {
private:
	nk::TMemo* value;
public:
	MemoStreamBuf(nk::TMemo* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;

		if (boClean)
			value->data.clear();
	}

	virtual ~MemoStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (StreamBufBase<ty>::os.str().length() > 0) {
			value->data.emplace_back(StreamBufBase<ty>::os.str().c_str());
		}
		else {
			value->data.emplace_back(ty::strEmpty);
		}
	}
};
#else
static_assert(false, "unbekanntes Framework, MemoStreamBuf");
#endif

#if defined BUILD_WITH_VCL || defined BUILD_WITH_FMX
template <typename ty>
class LabelStreamBuf : public StreamBufBase<ty> {
private:
	TLabel* value;
public:
	LabelStreamBuf(TLabel* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boClean) {
#if defined BUILD_WITH_VCL
			value->Caption = L"";
#elif defined BUILD_WITH_FMX
			value->Text = L"";
#endif
		}
	}

	virtual ~LabelStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
#if defined BUILD_WITH_VCL
		value->Caption = StreamBufBase<ty>::os.str().c_str();
#elif defined BUILD_WITH_FMX
		value->Text = StreamBufBase<ty>::os.str().c_str();
#endif
	}
};
#elif defined BUILD_WITH_QT
template <typename ty>
class LabelStreamBuf : public StreamBufBase<ty> {
private:
	QLabel* value;
public:
	LabelStreamBuf(QLabel* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boClean) value->clear();
	}

	virtual ~LabelStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if constexpr (std::is_same<Wide, ty>::value)
			value->setText(QString::fromStdWString(StreamBufBase<ty>::os.str()));
		else if constexpr (std::is_same<Latin, ty>::value)
			value->setText(QString::fromLatin1(QByteArray::fromStdString(StreamBufBase<ty>::os.str())));
		else
			value->setText(QString::fromStdString(StreamBufBase<ty>::os.str()));
	}
};
#elif BUILD_WITH_NUKLEAR
template <typename ty>
class LabelStreamBuf : public StreamBufBase<ty> {
private:
	nk::TLabel* value;
public:
	LabelStreamBuf(nk::TLabel* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boClean) value->text->clear();
	}

	virtual ~LabelStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		value->text = std::string(StreamBufBase<ty>::os.str());
	}
};
#else
static_assert(false, "unbekanntes Framework, LabelStreamBuf");
#endif

#if defined BUILD_WITH_VCL
template <typename ty>
class StatusStreamBuf : public StreamBufBase<ty> {
private:
	TStatusBar* value;
public:
	StatusStreamBuf(TStatusBar* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		value->SimplePanel = true;
		if (boClean) value->SimpleText = L"";
	}

	virtual ~StatusStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		value->SimpleText = StreamBufBase<ty>::os.str().c_str();
	}
};
#elif defined BUILD_WITH_NUKLEAR
template <typename ty>
class StatusStreamBuf : public StreamBufBase<ty> {
private:
	nk::TStatusBar* value;
public:
	StatusStreamBuf(nk::TStatusBar* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boClean) value->text.clear();
	}

	virtual ~StatusStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		value->text = StreamBufBase<ty>::os.str().c_str();
	}
};
#else
static_assert(false, "no stream for statusstream-component defined");
#endif

#if defined BUILD_WITH_VCL || defined BUILD_WITH_FMX
template <typename ty>
class ListBoxStreamBuf : public StreamBufBase<ty> {
private:
	TListBox* value;
public:
	ListBoxStreamBuf(TListBox* para, bool boMultiSelect = false, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boClean) value->Items->Clear();
	}

	virtual ~ListBoxStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (StreamBufBase<ty>::os.str().length() > 0) {
			value->Items->Add(StreamBufBase<ty>::os.str().c_str());
		}
		else {
			value->Items->Add(L"");
		}
	}
};
#elif defined BUILD_WITH_QT
template <typename ty>
class ListBoxStreamBuf : public StreamBufBase<ty> {
private:
	QListWidget* value;
public:
	ListBoxStreamBuf(QListWidget* para, bool boMultiSelect = false, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boMultiSelect) value->setSelectionMode(QAbstractItemView::ExtendedSelection);
		else value->setSelectionMode(QAbstractItemView::SingleSelection);
		if (boClean) value->clear();
	}

	virtual ~ListBoxStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (StreamBufBase<ty>::os.str().length() > 0) {
			if constexpr (std::is_same<Wide, ty>::value)
				value->addItem(QString::fromStdWString(StreamBufBase<ty>::os.str()));
			else if constexpr (std::is_same<Latin, ty>::value)
				value->addItem(QString::fromLatin1(QByteArray::fromStdString(StreamBufBase<ty>::os.str())));
			else
				value->addItem(QString::fromStdString(StreamBufBase<ty>::os.str()));
		}
		else {
			value->addItem("");
		}
	}
};
#elif defined BUILD_WITH_NUKLEAR
template <typename ty>
class ListBoxStreamBuf : public StreamBufBase<ty> {
private:
	nk::TListbox* value;
public:
	ListBoxStreamBuf(nk::TListbox* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boClean) value->items.clear();
	}

	virtual ~ListBoxStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (StreamBufBase<ty>::os.str().length() > 0) {
			value->items.emplace_back(StreamBufBase<ty>::os.str().c_str());
		}
		else {
			value->items.emplace_back(ty::strEmpty);
		}
	}
};
#else
static_assert(false, "unbekanntes Framework, ListBoxStreamBuf");
#endif

#if defined BUILD_WITH_VCL || defined BUILD_WITH_FMX
template <typename ty>
class ComboBoxStreamBuf : public StreamBufBase<ty> {
private:
	TComboBox* value;
public:
	ComboBoxStreamBuf(TComboBox* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boClean) {
#if defined BUILD_WITH_VCL
			value->Text = L"";
			value->Items->Clear();
#elif defined BUILD_WITH_FMX
			value->Items->Clear();
#endif
		}
	}

	virtual ~ComboBoxStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (StreamBufBase<ty>::os.str().length() > 0) {
			value->Items->Add(StreamBufBase<ty>::os.str().c_str());
		}
		else {
			value->Items->Add(L"");
		}
	}
};
#elif defined BUILD_WITH_QT
template <typename ty>
class ComboBoxStreamBuf : public StreamBufBase<ty> {
private:
	QComboBox* value;
public:
	ComboBoxStreamBuf(QComboBox* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boClean)  value->clear();
	}

	virtual ~ComboBoxStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (StreamBufBase<ty>::os.str().length() > 0) {
			if constexpr (std::is_same<Wide, ty>::value)
				value->addItem(QString::fromStdString(StreamBufBase<ty>::os.str()));
			else if constexpr (std::is_same<Latin, ty>::value)
				value->addItem(QString::fromLatin1(QByteArray::fromStdString(StreamBufBase<ty>::os.str())));
			else
				value->addItem(QString::fromStdString(StreamBufBase<ty>::os.str()));
		}
		else {
			value->addItem("");
		}
	}
};
#elif defined BUILD_WITH_NUKLEAR
template <typename ty>
class ComboBoxStreamBuf : public StreamBufBase<ty> {
private:
	nk::TCombobox* value;
public:
	ComboBoxStreamBuf(nk::TCombobox* para, bool boClean = true) : StreamBufBase<ty>() {
		value = para;
		if (boClean) {
			value->text = "";
			value->items.clear();
		}
	}

	virtual ~ComboBoxStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (StreamBufBase<ty>::os.str().length() > 0) {
			value->items.emplace_back(StreamBufBase<ty>::os.str().c_str());
		}
		else {
			value->items.push_back(ty::strEmpty);
		}
	}
};
#else
static_assert(false, "unbekanntes Framework, ComboBoxStreamBuf");
#endif


#if defined BUILD_WITH_VCL
template <typename ty>
class ListViewStreamBuf : public ListStreamBufBase<ty> {
private:
	TListView* value;
	TListItem* item;
	bool boNewItem;
public:
	ListViewStreamBuf(TListView* para, std::vector<tplList<ty>> const& caps, bool boClean = true) : ListStreamBufBase<ty>(caps) {
		value = para;
		item = nullptr;
		value->ViewStyle = vsReport;
		value->RowSelect = true;
		value->MultiSelect = true;
		boNewItem = true;
		if (boClean) {
			value->Items->Clear();
			value->Columns->Clear();
			SetColumns();
		}
	}

	virtual ~ListViewStreamBuf(void) { value = nullptr; }

	virtual void Write(void) {
		if (boNewItem) {
			item = value->Items->Add();
			item->Caption = StreamBufBase<ty>::os.str().c_str();
			boNewItem = false;
		}
		else {
			item->SubItems->Add(StreamBufBase<ty>::os.str().c_str());
		}
	}
	virtual void NewLine(void) { boNewItem = true; }
private:
	void SetColumns(void) {
		static std::map<EMyAlignmentType, TAlignment> Align = {
					   { EMyAlignmentType::left,    taLeftJustify },
					   { EMyAlignmentType::right,   taRightJustify },
					   { EMyAlignmentType::center,  taCenter },
					   { EMyAlignmentType::unknown, taLeftJustify }
		};
		for (auto const& caption : ListStreamBufBase<ty>::captions) {
			TListColumn* lc = value->Columns->Add();
			lc->Caption = std::get<0>(caption).c_str();
			lc->Width = std::get<1>(caption);
			lc->Alignment = Align[std::get<2>(caption)];
		}
	}
};

#elif defined BUILD_WITH_FMX

template <typename ty>
class ListViewStreamBuf : public ListStreamBufBase<ty> {
private:
	TStringGrid* sg = nullptr;
	int          iColumn = 0;
	int          iRow = 0;
public:
	ListViewStreamBuf(TStringGrid *para, std::vector<tplList<ty>> const& caps, bool boClean = true) : ListStreamBufBase<ty>(caps) {
		sg = para;
		iColumn = 0;
		iRow = 0;

		sg->ReadOnly = true;
		sg->Options <<= TGridOption::RowSelect;
		sg->Options >>= TGridOption::ColumnMove;
		sg->Options <<= TGridOption::AlwaysShowSelection;
		if (boClean) {
			sg->BeginUpdate();
			for (int i = 0; i < sg->ColumnCount; ++i)
				for (int j = 0; j < sg->RowCount; ++j) sg->Cells[i][j] = L"";
			sg->RowCount = 0;
			sg->ClearColumns();
			sg->EndUpdate();
			AddColumns();
		}
	}

	virtual ~ListViewStreamBuf(void) { sg = nullptr; }

	virtual void NewLine(void) { iColumn = 0; }

	virtual void Write(void) {
		if (iColumn == 0) {
			if (iRow == 0) {
				iRow = 1;
				sg->RowCount = 1;  // stringgrid are terrible coded, be careful
			}
			else if (iRow > sg->RowCount) {
				iRow = 1;
				sg->RowCount = 1;  // only row because column empty
			}
			else {
				sg->RowCount += 1;
				iRow = sg->RowCount;
			}
		}
		sg->Cells[iColumn++][iRow - 1] = StreamBufBase<ty>::os.str().c_str();
	}
private:
	void AddColumns() {
		static std::map<EMyAlignmentType, TTextAlign> Align = {
					   { EMyAlignmentType::left,    TTextAlign::Leading },
					   { EMyAlignmentType::right,   TTextAlign::Trailing },
					   { EMyAlignmentType::center,  TTextAlign::Center },
					   { EMyAlignmentType::unknown, TTextAlign::Leading }
		};
		for (auto const& caption : ListStreamBufBase<ty>::captions) {
			TStringColumn* col = new TStringColumn(sg);
			sg->AddObject(col);
			col->HorzAlign = Align[std::get<2>(caption)];
			col->Header = std::get<0>(caption).c_str();
			col->Width = std::get<1>(caption);
		}
	}
};


#elif defined BUILD_WITH_QT

template <typename ty>
class ListViewStreamBuf : public ListStreamBufBase<ty> {
private:
	QTableWidget* tw = nullptr;
	int          iColumn = 0;
	int          iRow = 0;
	static inline std::map<EMyAlignmentType, Qt::Alignment> Align = {
										 { EMyAlignmentType::left,    Qt::AlignVCenter | Qt::AlignLeft },
										 { EMyAlignmentType::right,   Qt::AlignVCenter | Qt::AlignRight },
										 { EMyAlignmentType::center,  Qt::AlignVCenter | Qt::AlignCenter },
										 { EMyAlignmentType::unknown, Qt::AlignVCenter | Qt::AlignLeft }
	};


public:
	ListViewStreamBuf(QTableWidget* para, std::vector<tplList<ty>> const& caps, bool boClean = true) : ListStreamBufBase<ty>(caps) {
		tw = para;
		iColumn = 0;
		iRow = 0;

		tw->verticalHeader()->setVisible(false);
		tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
		tw->setSelectionBehavior(QAbstractItemView::SelectRows);
		tw->setSelectionMode(QAbstractItemView::ExtendedSelection); // SingleSelection
		tw->setShowGrid(false);

		if (boClean) {
			tw->clear();
			QStringList m_TableHeader;
			tw->setColumnCount(ListStreamBufBase<ty>::captions.size());
			for (int i = 0; i < ListStreamBufBase<ty>::captions.size(); ++i) {
				tplList<Narrow> const& caption = ListStreamBufBase<ty>::captions[i];
				QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(std::get<0>(caption)));
				item->setTextAlignment(Align[std::get<2>(caption)]);
				tw->setHorizontalHeaderItem(i, item);
				tw->setColumnWidth(i, std::get<1>(caption));
			}
			tw->resizeRowsToContents();
		}
	}

	virtual ~ListViewStreamBuf(void) { tw = nullptr; }

	virtual void NewLine(void) { iColumn = 0; }

	virtual void Write(void) {
		if (iColumn == 0) {
			if (iRow == 0) {
				iRow = 1;
				tw->setRowCount(1);
			}
			else if (iRow > tw->rowCount()) {
				iRow = tw->rowCount() + 1;
				tw->setRowCount(iRow);  // only row because column empty
			}
			else {
				tw->setRowCount(tw->rowCount() + 1);
				iRow = tw->rowCount();
			}
		}
		tplList<Narrow> const& caption = ListStreamBufBase<ty>::captions[iColumn];
		QTableWidgetItem* item;
		if constexpr (std::is_same<Wide, ty>::value)
			item = new QTableWidgetItem(QString::fromStdString(StreamBufBase<ty>::os.str()));
		else if constexpr (std::is_same<Latin, ty>::value)
			item = new QTableWidgetItem(QString::fromLatin1(QByteArray::fromStdString(StreamBufBase<ty>::os.str())));
		else
			item = new QTableWidgetItem(QString::fromStdString(StreamBufBase<ty>::os.str()));
		item->setTextAlignment(Align[std::get<2>(caption)]);
		tw->setItem(iRow - 1, iColumn++, item);
		//tw->resizeRowsToContents(); // performance killer
		//tw->resizeColumnsToContents();
	}
};

#elif defined BUILD_WITH_NUKLEAR
template <typename ty>
class ListViewStreamBuf : public ListStreamBufBase<ty> {
private:
	nk::TGrid* tw = nullptr;
	int          iColumn = 0;
	int          iRow = 0;
	static inline std::map<EMyAlignmentType, nk_text_alignment> Align = {
										 { EMyAlignmentType::left,    NK_TEXT_LEFT },
										 { EMyAlignmentType::right,   NK_TEXT_RIGHT },
										 { EMyAlignmentType::center,  NK_TEXT_CENTERED },
										 { EMyAlignmentType::unknown, NK_TEXT_LEFT }
	};


public:
	ListViewStreamBuf(nk::TGrid* para, std::vector<tplList<ty>> const& caps, bool boClean = true) : ListStreamBufBase<ty>(caps) {
		tw = para;
		iColumn = 0;
		iRow = 0;

		if (boClean) {
			tw->clear();
			tw->Columns.resize(ListStreamBufBase<ty>::captions.size());
			for (int i = 0; i < ListStreamBufBase<ty>::captions.size(); ++i) {
				tplList<Narrow> const& caption = ListStreamBufBase<ty>::captions[i];
				
				nk::TGrid::THeadItem& head = tw->Columns[i];
				head.caption= std::get<0>(caption);
				head.alignment = std::get<2>(caption);
				head.nk_alignment=(Align[std::get<2>(caption)]);
				head.width=std::get<1>(caption);
			}
		}
	}

	virtual ~ListViewStreamBuf(void) { tw = nullptr; }

	virtual void NewLine(void) { iColumn = 0; }

	virtual void Write(void) {
		if (iColumn == 0) {
			if (iRow == 0) {
				iRow = 1;
				tw->Rows.resize(1);
			}
			else if (iRow > tw->rowCount()) {
				iRow = tw->rowCount() + 1;
				tw->Rows.resize(iRow);  // only row because column empty
			}
			else {
				tw->Rows.resize(tw->rowCount() + 1);
				iRow = tw->rowCount();
			}
		}
		tplList<Narrow> const& caption = ListStreamBufBase<ty>::captions[iColumn];
		
		//achtung, iRow ist 1-basiert
		std::vector<std::string>& Row = tw->Rows[iRow - 1];
		if (Row.size() < tw->Columns.size())
		{
			Row.resize(tw->Columns.size());
		}
		Row[iColumn++] = StreamBufBase<ty>::os.str();
	}
};
#else
static_assert(false, "unbekanntes Framework, ListViewStreamBuf");
#endif


template <typename ty_base>
class TStreamWrapper {
private:
	typename ty_base::stream_type& str;
	typename ty_base::streambuf    *old = nullptr;
public:
	TStreamWrapper() = delete;
	TStreamWrapper(TStreamWrapper const&) = delete;
	TStreamWrapper(TStreamWrapper && ref) {
		std::swap(str, ref.str);
		std::swap(old, ref.old);
	}

	TStreamWrapper(typename ty_base::stream_type& ref) : str(ref) { old = nullptr; }
	~TStreamWrapper(void) {
		if (old != nullptr) {
			typename ty_base::streambuf* tmp = str.rdbuf(old);
			// if(tmp) delete tmp;
		}
	}

	void SafeReset(void) {
		if (old != nullptr) {
			typename ty_base::streambuf* tmp = str.rdbuf(old);
			// if(tmp) delete tmp;
		}
	}

	void Reset(void) {
		if (old != nullptr) {
			typename ty_base::streambuf* tmp = str.rdbuf(old);
			// if(tmp) delete tmp;   Fehler im BorlandMM
		}
	}

#if defined BUILD_WITH_VCL || defined BUILD_WITH_FMX
	void Activate(TMemo* elem) {
		Reset();
		old = str.rdbuf(new MemoStreamBuf<ty_base>(elem));
	}
#elif defined BUILD_WITH_QT
	void Activate(QTextEdit* elem) {
		Reset();
		old = str.rdbuf(new MemoStreamBuf<ty_base>(elem));
	}
#elif defined BUILD_WITH_NUKLEAR
	void Activate(nk::TMemo* elem) {
		Reset();
		old = str.rdbuf(new MemoStreamBuf<ty_base>(elem));
	}
#else
	static_assert(false, "Missing Activate(TMemo)");
#endif

#if defined BUILD_WITH_VCL || defined BUILD_WITH_FMX
	void Activate(TLabel* elem) {
		Reset();
		old = str.rdbuf(new LabelStreamBuf<ty_base>(elem));
	}
#elif defined BUILD_WITH_QT
	void Activate(QLabel* elem) {
		Reset();
		old = str.rdbuf(new LabelStreamBuf<ty_base>(elem));
	}
#elif defined BUILD_WITH_NUKLEAR
	void Activate(nk::TLabel* elem) {
		Reset();
		old = str.rdbuf(new LabelStreamBuf<ty_base>(elem));
	}
#else
	static_assert(false, "Missing Activate(TLabel)");
#endif

#if defined BUILD_WITH_VCL
	void Activate(TStatusBar* elem) {
		Reset();
		old = str.rdbuf(new StatusStreamBuf<ty_base>(elem));
	}
#elif defined BUILD_WITH_NUKLEAR
void Activate(nk::TStatusBar* elem) {
	Reset();
	old = str.rdbuf(new StatusStreamBuf<ty_base>(elem));
}
#else
	static_assert(false, "Missing Activate(TStatusBar)");
#endif

#if defined BUILD_WITH_VCL || defined BUILD_WITH_FMX
	void Activate(TListBox* elem) {
		Reset();
		old = str.rdbuf(new ListBoxStreamBuf<ty_base>(elem));
	}
#elif defined BUILD_WITH_QT
	void Activate(QListWidget* elem) {
		Reset();
		old = str.rdbuf(new ListBoxStreamBuf<ty_base>(elem));
	}
#elif defined BUILD_WITH_NUKLEAR
	void Activate(nk::TListbox* elem) {
		Reset();
		old = str.rdbuf(new ListBoxStreamBuf<ty_base>(elem));
	}
#else
	static_assert(false, "Missing Activate(TListBox)");
#endif

#if defined BUILD_WITH_VCL || defined BUILD_WITH_FMX
	void Activate(TComboBox* elem) {
		Reset();
		old = str.rdbuf(new ComboBoxStreamBuf<ty_base>(elem));
	}
#elif defined BUILD_WITH_QT
	void Activate(QComboBox* elem) {
		Reset();
		old = str.rdbuf(new ComboBoxStreamBuf<ty_base>(elem));
	}
#elif defined BUILD_WITH_NUKLEAR
	void Activate(nk::TCombobox* elem) {
		Reset();
		old = str.rdbuf(new ComboBoxStreamBuf<ty_base>(elem));
	}
#else
	static_assert(false, "Missing Activate(TComboBox)");
#endif

#if defined BUILD_WITH_VCL
	void Activate(TListView* elem, std::vector<tplList<ty_base>> const& caps) {
		Reset();
		old = str.rdbuf(new ListViewStreamBuf<ty_base>(elem, caps));
	}
#elif defined BUILD_WITH_FMX
	void Activate(TStringGrid* elem, std::vector<tplList<ty_base>> const& caps) {
		Reset();
		old = str.rdbuf(new ListViewStreamBuf<ty_base>(elem, caps));
	}
#elif defined BUILD_WITH_QT
	void Activate(QTableWidget* elem, std::vector<tplList<ty_base>> const& caps) {
		Reset();
		old = str.rdbuf(new ListViewStreamBuf<ty_base>(elem, caps));
	}
#elif defined BUILD_WITH_NUKLEAR
	void Activate(nk::TGrid* elem, std::vector<tplList<ty_base>> const& caps) {
		Reset();
		old = str.rdbuf(new ListViewStreamBuf<ty_base>(elem, caps));
	}
#else
	static_assert(false, "Missing Activate(TListView)");
#endif



	void Activate(typename ty_base::streambuf* element) {
		Reset();
		old = str.rdbuf(element);
	}

};


#endif

