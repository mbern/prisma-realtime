/*
 * prisma-realtime analyzes and visualizes monophonic musical instrument tones
 * Copyright (C) 2014  Michael Bernhard
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef GL_GRAPH_BASE_H
#define GL_GRAPH_BASE_H

#include <GL/glew.h>

#include <qthread.h>
#include <qgl.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include <QPaintEvent>

struct CVertex														// Vertex Class
{
	GLfloat x;													// X Component
	GLfloat y;													// Y Component
	GLfloat z;													// Z Component
};

struct CColor
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
};

struct CFeature
{
	GLfloat f0;
	GLfloat max;
};

struct CInterleaved
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

class GLGraphBase : public QGLWidget
{
public:
	GLGraphBase( const QGLFormat & format,
		         QWidget * parent = 0,
				 const char * name = 0,
				 const QGLWidget * shareWidget = 0,
				 Qt::WFlags f = 0 )
				 : QGLWidget( format, parent, name, shareWidget, f ), m_UpdateThread( this ), m_WindowSize(0,0),
				   m_alreadyStarted(false)

	{
	}

	~GLGraphBase( )
	{
		stop();
	}

	void start()
	{
		if( !m_alreadyStarted )
		{
			m_UpdateThread.start( QThread::NormalPriority );
			m_alreadyStarted = true;
		}
	}

	void stop()
	{
		if( m_alreadyStarted )
		{
			m_UpdateThread.stop();
			m_UpdateThread.wait();
			m_alreadyStarted = false;
		}
	}

protected:
	void resizeEvent(QResizeEvent *evt)
	{
		m_WindowSize = evt->size();
        m_UpdateThread.resizeViewport( m_WindowSize );

	}

    void paintEvent(QPaintEvent *)
	{
	}

    void closeEvent(QCloseEvent *evt)
    {
		if( m_alreadyStarted )
		{
			m_UpdateThread.stop();
			m_UpdateThread.wait();
			m_alreadyStarted = false;
		}
        QGLWidget::closeEvent(evt);
    }

	void showEvent( QShowEvent * evt )
	{
		(void) evt; // surpress warning
		if( !m_alreadyStarted )
		{
			m_UpdateThread.start( QThread::NormalPriority );
			m_alreadyStarted = true;
		}
	}

	virtual void myResizeGL( int w, int h ) = 0;
	virtual void myPaintGL( double offset ) = 0;
	virtual void myUpdateStartGL() = 0;
	virtual int myUpdateDataGL() = 0;
	virtual void myInitializeGL() = 0;
	virtual void myCleanupGL() = 0;

	virtual double getTime() = 0;
	virtual bool getRunState() = 0;

private:

	class GLPaintThread : public QThread
	{
	public:
		GLPaintThread( GLGraphBase *GlWidget ) : glw(GlWidget)
		{
		}

        void resizeViewport(const QSize &size)
		{
			w = size.width();
			h = size.height();
			doResize = true;
		}

		void updateView()
		{
			doResize = true;
		}

        void run()
		{
			doRendering = true;

			int retval = 0;
			double currentsampletime;
			int n;

			glw->setAutoBufferSwap( false );

			glw->makeCurrent();
			glw->myInitializeGL();

			while( doRendering )
			{
				// Synchronize to audio stream
				currentsampletime = glw->getTime();

				// Update data to paint
				glw->myUpdateStartGL();
				n = 0;
				do
				{
					retval = glw->myUpdateDataGL();
					n++;
				}
				while(retval == TRUE && n < 5);

				// Check if we need to resize the output
				if( doResize )
				{
					glw->myResizeGL( w, h );
					doResize = false;
				}

				// Paint the graphs
				glw->myPaintGL(currentsampletime-.1);

				// Swap to a new buffer
				glw->swapBuffers();
			}

			glw->myCleanupGL();
			glw->doneCurrent();
		}

        void stop()
		{
			doRendering = false;
		}

		bool isRendering()
		{
			return doRendering;
		}

	private:
		int w,h;
		bool doRendering;
		bool doResize;
		GLGraphBase *glw;
	};

	GLPaintThread m_UpdateThread;

protected:

	QSize m_WindowSize;

	private:
		bool m_alreadyStarted;

};



#endif // GL_GRAPH_BASE_H
