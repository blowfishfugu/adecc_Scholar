#pragma once
#ifndef MYFORMLIST_H
#define MYFORMLIST_H

#include <MyStdTypes.h>
#include <MyForm.h>

template <EMyFrameworkType ft, typename ty>
struct my_formlist_iterator {
	using iterator_category = std::input_iterator_tag;
	using value_type = ty;
	using difference_type = std::ptrdiff_t;
	using reference = value_type & ;
	using pointer = const value_type*;

	my_formlist_iterator() = default;

	my_formlist_iterator(TMyForm* frm, std::string const& fld) :
		form(frm), strField(fld) {
		++* this;
	}


	my_formlist_iterator& operator = (std::pair<TMyForm*, std::string> para) {
		form = para.first;
		strField = para.second;
		start_pos = 0;
		return *this;
	}

	my_formlist_iterator& operator = (my_formlist_iterator const& ref) {
		form = ref.form;
		strField = ref.strField;
		start_pos = ref.start_pos;
		return *this;
	}

	value_type operator*() const {
		if (!form) throw std::runtime_error("unexpected error in my_formlist_iterator, form is null");
		return form->Get_Value_in_list<ft, ty>(strField, start_pos - 1);
	}
	//pointer operator->() const { return &theLine; }


	my_formlist_iterator& operator++() {
		if (form) {
			if (start_pos < form->Count_in_list<ft>(strField)) start_pos++;
			else {
				form = nullptr;
				strField = "";
			}
		}
		return *this;
	}

	my_formlist_iterator operator++(int) {
		auto copy(*this);
		++* this;
		return copy;
	}

	friend bool operator==(const my_formlist_iterator& x, const my_formlist_iterator& y) {
		if (x.form == nullptr && y.form == nullptr) return true;
		if (x.form == nullptr || y.form == nullptr) return false;
		if (x.form == y.form) {
			return x.strField == y.strField;
		}
		else return false;
	}

	friend bool operator!=(const my_formlist_iterator& x, const my_formlist_iterator& y) {
		return !(x == y);
	}

private:
	TMyForm *form = nullptr;
	std::string strField = "";
	size_t start_pos = 0;

};


template <EMyFrameworkType ft, typename ty>
struct my_formlist {
	using const_iterator = my_formlist_iterator<ft, ty>;
	my_formlist(TMyForm* frm, std::string const& fld) : form(frm), strField(fld) { }
	my_formlist(my_formlist<ft, ty> const& ref) : form(ref.form), strField(ref.strField) { }
	my_formlist_iterator<ft, ty> begin() { return my_formlist_iterator<ft, ty>(form, strField); }
	my_formlist_iterator<ft, ty> end() { return my_formlist_iterator<ft, ty>(); }
	my_formlist_iterator<ft, ty> cbegin() const { return my_formlist_iterator<ft, ty>(form, strField); }
	my_formlist_iterator<ft, ty> cend() const { return my_formlist_iterator<ft, ty>(); }
	size_t size(void) const {
		if (!form) return 0u;
		return form->Count_in_list<ft>(strField);
	}

private:
	TMyForm *form;
	std::string strField;
};

#endif