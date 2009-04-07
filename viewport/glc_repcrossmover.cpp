/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.1.0, packaged on March, 2009.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/

#include "glc_repcrossmover.h"
#include "glc_viewport.h"

GLC_RepCrossMover::GLC_RepCrossMover(GLC_Viewport* pViewport)
: GLC_RepMover(pViewport)
{

}

GLC_RepCrossMover::~GLC_RepCrossMover()
{

}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Virtual interface for OpenGL Geometry set up.
void GLC_RepCrossMover::glDraw()
{
	int nLgAxe;
	const int winHSize= m_pViewport->getWinHSize();
	const int winVSize= m_pViewport->getWinVSize();
	if (winHSize > winVSize)
	{
		nLgAxe = static_cast<int>(static_cast<double>(winVSize) / 2.0);
	}
	else
	{
		nLgAxe = static_cast<int>(static_cast<double>(winHSize) / 2.0);
	}

	// Compute the length of camera's field of view
	const double ChampsVision = 2 * (m_pViewport->cameraHandle()->getDistEyeTarget()) *  tan((m_pViewport->getFov() * glc::PI / 180.0) / 2.0);

	// the side of camera's square is mapped on Vertical length of window
	// Axis length in OpenGL unit = length(Pixel) * (dimend GL / dimens Pixel)
	const double dLgAxe= ((double)nLgAxe * ChampsVision / (double)winVSize) / 7;
	const double dDecAxe= dLgAxe / 3;
	glPushMatrix();

	glTranslated(m_pViewport->cameraHandle()->getTarget().X(), m_pViewport->cameraHandle()->getTarget().Y(),
			m_pViewport->cameraHandle()->getTarget().Z() );

	// Graphic propertys
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4d(m_MainColor.redF(), m_MainColor.greenF(), m_MainColor.blueF(), m_MainColor.alphaF());
	glLineWidth(1.0);

	// Display camera's target lines
	glBegin(GL_LINES);
		//X axis
		glVertex3d(-dLgAxe, 0, 0);
		glVertex3d(-dDecAxe, 0, 0);
		glVertex3d(dDecAxe, 0, 0);
		glVertex3d(dLgAxe, 0, 0);

		//Y axis
		glVertex3d(0, -dLgAxe, 0);
		glVertex3d(0, -dDecAxe, 0);
		glVertex3d(0, dDecAxe, 0);
		glVertex3d(0, dLgAxe, 0);

		//Z axis
		glVertex3d(0, 0, -dLgAxe);
		glVertex3d(0, 0, -dDecAxe);
		glVertex3d(0, 0, dDecAxe);
		glVertex3d(0, 0, dLgAxe);

	glEnd();

	glPopMatrix();

}


