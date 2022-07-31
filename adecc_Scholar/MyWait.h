#pragma once
#ifndef MYWAIT_H
#define MYWAIT_H

#if defined BUILD_WITH_VCL
class TMyWait {
private:
	TCursor old_cursor;
public:
	TMyWait(void) {
		old_cursor = Screen->Cursor;
		Screen->Cursor = crHourGlass;
	}

	TMyWait(TMyWait const&) = delete;
	~TMyWait(void) {
		Screen->Cursor = old_cursor;
	}
};


#elif defined BUILD_WITH_FMX
class TMyWait {
private:
	TCursor old_cursor;
	_di_IFMXCursorService cs;

public:
	TMyWait(void) {
		if (TPlatformServices::Current->SupportsPlatformService(__uuidof(IFMXCursorService), (void*)&cs)) {
			old_cursor = cs->GetCursor();
			cs->SetCursor(crHourGlass);
		}
	}

	TMyWait(TMyWait const&) = delete;
	~TMyWait(void) {
		cs->SetCursor(old_cursor);
	}
};
#elif defined BUILD_WITH_QT
class TMyWait {
private:
	QWidget* currentWidget = nullptr;
public:
	TMyWait(void) {
		currentWidget = QApplication::activeWindow();
		if (currentWidget) {
			currentWidget->setCursor(Qt::WaitCursor);
		}
	}
	TMyWait(TMyWait const&) = delete;
	~TMyWait(void) {
		if (currentWidget) currentWidget->unsetCursor();
	}
};
#elif defined BUILD_WITH_NUKLEAR
class TMyWait
{
private:
	//platform-specific
	//CursorHandle	//a handle to previous cursor
	//WindowContext //a window or device-context
#pragma message("Set/Reset of Cursors not implemented")
public:
	TMyWait(void) = default;
	TMyWait(TMyWait const&) = delete;
	~TMyWait(void) = default;
};
#else
static_assert( false, " "Fehlende Klasse TMyWait in diesem Framework")
#endif


#endif