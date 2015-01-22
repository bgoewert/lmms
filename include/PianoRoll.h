/*
 * PianoRoll.h - declaration of class PianoRoll which is a window where you
 *               can set and edit notes in an easy way
 *
 * Copyright (c) 2004-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2008 Andrew Kelley <superjoe30/at/gmail/dot/com>
 *
 * This file is part of LMMS - http://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef PIANO_ROLL_H
#define PIANO_ROLL_H

#include <QWidget>
#include <QInputDialog>

#include "Editor.h"
#include "ComboBoxModel.h"
#include "SerializingObject.h"
#include "Note.h"
#include "lmms_basics.h"
#include "Song.h"
#include "ToolTip.h"

class QPainter;
class QPixmap;
class QScrollBar;
class QString;
class QMenu;
class QSignalMapper;

class ComboBox;
class NotePlayHandle;
class Pattern;
class TimeLineWidget;

class PianoRoll : public QWidget
{
	Q_OBJECT
	Q_PROPERTY( QColor gridColor READ gridColor WRITE setGridColor )
	Q_PROPERTY( QColor noteModeColor READ noteModeColor WRITE setNoteModeColor )
	Q_PROPERTY( QColor noteColor READ noteColor WRITE setNoteColor )
	Q_PROPERTY( QColor barColor READ barColor WRITE setBarColor )
public:
	enum EditModes
	{
		ModeDraw,
		ModeErase,
		ModeSelect,
		ModeEditDetuning,
	};

	/*! \brief Resets settings to default when e.g. creating a new project */
	void reset();

	void setCurrentPattern( Pattern* newPattern );

	inline void stopRecording()
	{
		m_recording = false;
	}

	inline bool isRecording() const
	{
		return m_recording;
	}

	const Pattern* currentPattern() const
	{
		return m_pattern;
	}

	bool hasValidPattern() const
	{
		return m_pattern != NULL;
	}

	Song::PlayModes desiredPlayModeForAccompany() const;

	int quantization() const;
	
	// qproperty acces functions
	QColor gridColor() const;
	void setGridColor( const QColor & c );
	QColor noteModeColor() const;
	void setNoteModeColor( const QColor & c );
	QColor noteColor() const;
	void setNoteColor( const QColor & c );
	QColor barColor() const;
	void setBarColor( const QColor & c );


protected:
	virtual void keyPressEvent( QKeyEvent * ke );
	virtual void keyReleaseEvent( QKeyEvent * ke );
	virtual void leaveEvent( QEvent * e );
	virtual void mousePressEvent( QMouseEvent * me );
	virtual void mouseDoubleClickEvent( QMouseEvent * me );
	virtual void mouseReleaseEvent( QMouseEvent * me );
	virtual void mouseMoveEvent( QMouseEvent * me );
	virtual void paintEvent( QPaintEvent * pe );
	virtual void resizeEvent( QResizeEvent * re );
	virtual void wheelEvent( QWheelEvent * we );

	int getKey( int y ) const;
	static inline void drawNoteRect( QPainter & p, int x, int y,
					int  width, Note * n, const QColor & noteCol );
	void removeSelection();
	void selectAll();
	void getSelectedNotes( NoteVector & selected_notes );

	// for entering values with dblclick in the vol/pan bars
	void enterValue( NoteVector* nv );

protected slots:
	void play();
	void record();
	void recordAccompany();
	void stop();

	void startRecordNote( const Note & n );
	void finishRecordNote( const Note & n );

	void horScrolled( int new_pos );
	void verScrolled( int new_pos );

	void setEditMode(int mode);

	void copySelectedNotes();
	void cutSelectedNotes();
	void pasteNotes();
	void deleteSelectedNotes();

	void updatePosition(const MidiTime & t );
	void updatePositionAccompany(const MidiTime & t );

	void zoomingChanged();
	void quantizeChanged();

	void updateSemiToneMarkerMenu();

	void changeNoteEditMode( int i );
	void markSemiTone( int i );

	void hidePattern( Pattern* pattern );

	void selectRegionFromPixels( int x, int y );


signals:
	void currentPatternChanged();
	void semiToneMarkerMenuScaleSetEnabled(bool);
	void semiToneMarkerMenuChordSetEnabled(bool);


private:
	enum Actions
	{
		ActionNone,
		ActionMoveNote,
		ActionResizeNote,
		ActionSelectNotes,
		ActionChangeNoteProperty,
		ActionResizeNoteEditArea
	};

	enum NoteEditMode
	{
		NoteEditVolume,
		NoteEditPanning,
		NoteEditCount // make sure this one is always last
	};

	enum SemiToneMarkerAction
	{
		stmaUnmarkAll,
		stmaMarkCurrentSemiTone,
		stmaMarkCurrentScale,
		stmaMarkCurrentChord,
	};

	enum PianoRollKeyTypes
	{
		PR_WHITE_KEY_SMALL,
		PR_WHITE_KEY_BIG,
		PR_BLACK_KEY
	};

	QVector<QString> m_nemStr; // gui names of each edit mode
	QMenu * m_noteEditMenu; // when you right click below the key area

	QList<int> m_markedSemiTones;
	QMenu * m_semiToneMarkerMenu; // when you right click on the key area

	PianoRoll();
	PianoRoll( const PianoRoll & );
	virtual ~PianoRoll();

	void autoScroll(const MidiTime & t );

	MidiTime newNoteLen() const;

	void shiftPos(int amount);
	void shiftSemiTone(int amount);
	bool isSelection() const;
	int selectionCount() const;
	void testPlayNote( Note * n );
	void testPlayKey( int _key, int _vol, int _pan );
	void pauseTestNotes(bool pause = true );

	inline int noteEditTop() const;
	inline int keyAreaBottom() const;
	inline int noteEditBottom() const;
	inline int keyAreaTop() const;
	inline int noteEditRight() const;
	inline int noteEditLeft() const;

	void dragNotes( int x, int y, bool alt, bool shift );

	static const int cm_scrollAmtHoriz = 10;
	static const int cm_scrollAmtVert = 1;

	static QPixmap * s_whiteKeyBigPm;
	static QPixmap * s_whiteKeyBigPressedPm;
	static QPixmap * s_whiteKeySmallPm;
	static QPixmap * s_whiteKeySmallPressedPm;
	static QPixmap * s_blackKeyPm;
	static QPixmap * s_blackKeyPressedPm;
	static QPixmap * s_toolDraw;
	static QPixmap * s_toolErase;
	static QPixmap * s_toolSelect;
	static QPixmap * s_toolMove;
	static QPixmap * s_toolOpen;

	static PianoRollKeyTypes prKeyOrder[];

	static TextFloat * s_textFloat;

	ComboBoxModel m_zoomingModel;
	ComboBoxModel m_quantizeModel;
	ComboBoxModel m_noteLenModel;
	ComboBoxModel m_scaleModel;
	ComboBoxModel m_chordModel;


	Pattern* m_pattern;
	QScrollBar * m_leftRightScroll;
	QScrollBar * m_topBottomScroll;

	MidiTime m_currentPosition;
	bool m_recording;
	QList<Note> m_recordingNotes;

	Note * m_currentNote;
	Actions m_action;
	NoteEditMode m_noteEditMode;

	int m_selectStartTick;
	int m_selectedTick;
	int m_selectStartKey;
	int m_selectedKeys;

	// boundary box around all selected notes when dragging
	int m_moveBoundaryLeft;
	int m_moveBoundaryTop;
	int m_moveBoundaryRight;
	int m_moveBoundaryBottom;

	// remember where the scrolling started when dragging so that
	// we can handle dragging while scrolling with arrow keys
	int m_mouseDownKey;
	int m_mouseDownTick;

	// remember the last x and y of a mouse movement
	int m_lastMouseX;
	int m_lastMouseY;

	// x,y of when the user starts a drag
	int m_moveStartX;
	int m_moveStartY;

	int m_oldNotesEditHeight;
	int m_notesEditHeight;
	int m_ppt;  // pixels per tact
	int m_totalKeysToScroll;

	// remember these values to use them
	// for the next note that is set
	MidiTime m_lenOfNewNotes;
	volume_t m_lastNoteVolume;
	panning_t m_lastNotePanning;

	int m_startKey; // first key when drawing
	int m_lastKey;

	EditModes m_editMode;
	EditModes m_ctrlMode; // mode they were in before they hit ctrl

	bool m_mouseDownLeft; //true if left click is being held down
	bool m_mouseDownRight; //true if right click is being held down

	TimeLineWidget * m_timeLine;
	bool m_scrollBack;

	void copy_to_clipboard(const NoteVector & notes ) const;

	void drawDetuningInfo( QPainter & _p, Note * _n, int _x, int _y );
	bool mouseOverNote();
	Note * noteUnderMouse();

	// turn a selection rectangle into selected notes
	void computeSelectedNotes( bool shift );
	void clearSelectedNotes();

	// did we start a mouseclick with shift pressed
	bool m_startedWithShift;

	friend class Engine;
	friend class PianoRollWindow;

	// qproperty fields
	QColor m_gridColor;
	QColor m_noteModeColor;
	QColor m_noteColor;
	QColor m_barColor;

signals:
	void positionChanged( const MidiTime & );

} ;


class PianoRollWindow : public Editor, SerializingObject
{
	Q_OBJECT
public:
	PianoRollWindow();

	const Pattern* currentPattern() const;
	void setCurrentPattern(Pattern* pattern);

	int quantization() const;

	void play();
	void stop();
	void record();
	void recordAccompany();
	void stopRecording();

	bool isRecording() const;

	/*! \brief Resets settings to default when e.g. creating a new project */
	void reset();

	using SerializingObject::saveState;
	using SerializingObject::restoreState;
	virtual void saveSettings(QDomDocument & doc, QDomElement & de );
	virtual void loadSettings( const QDomElement & de );

	inline virtual QString nodeName() const
	{
		return "pianoroll";
	}

	QSize sizeHint() const;

signals:
	void currentPatternChanged();

private:
	PianoRoll* m_editor;

	ComboBox * m_zoomingComboBox;
	ComboBox * m_quantizeComboBox;
	ComboBox * m_noteLenComboBox;
	ComboBox * m_scaleComboBox;
	ComboBox * m_chordComboBox;

};


#endif

