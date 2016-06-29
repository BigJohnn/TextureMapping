////////////////////////////////////////////////////////////
//
//  WINGED.CPP - Winged Edge Boundary Representation Class
//
//  Version:    1.00B
//
//  History:    96/05/05 - Version 1.00A release.
//              97/04/23 - Modified KillEdgeFace to add
//                         copies of edge links to remaining
//                         face edge link list.
//              97/04/24 - Version 1.00B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 5.0
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 1996-1997 byHeart Consultants Limited
//
//  The following source code may be freely copied,
//  redistributed, and/or modified for personal use ONLY,
//  as long as the copyright notice is included with all
//  source code files.
//
////////////////////////////////////////////////////////////

//P.S. Some changes are applyed to solve our problem

#include "stdafx.h"
#include "WingedEdge.h"

// Get next edge around face
WEdge *WFace::GetNextEdge(WEdge *pe, BOOL cw_dir)
{
	WEdge *pne;   // Next edge pointer

	if (cw_dir == TRUE)   // Clockwise direction ?
	{
		if (pe->GetLeftFacePtr() == this)
			pne = pe->GetTailLeftWingPtr();
		else
			pne = pe->GetHeadRightWingPtr();
	}
	else  // Counterclockwise direction
	{
		if (pe->GetLeftFacePtr() == this)
			pne = pe->GetHeadLeftWingPtr();
		else
			pne = pe->GetTailRightWingPtr();
	}

	return pne;
}

// Set edge wing pointers
void WFace::SetWings(WEdge *pea, WEdge *peb)
{
	if (pea->GetTailVertexPtr() ==
		peb->GetTailVertexPtr())
	{
		if (pea->GetLeftFacePtr() == this &&
			peb->GetRightFacePtr() == this)
		{
			pea->SetTailLeftWingPtr(peb);
			peb->SetTailRightWingPtr(pea);
		}
		else
		{
			pea->SetTailRightWingPtr(peb);
			peb->SetTailLeftWingPtr(pea);
		}
	}
	else if (pea->GetTailVertexPtr() ==
		peb->GetHeadVertexPtr())
	{
		if (pea->GetLeftFacePtr() == this &&
			peb->GetLeftFacePtr() == this)
		{
			pea->SetTailLeftWingPtr(peb);
			peb->SetHeadLeftWingPtr(pea);
		}
		else
		{
			pea->SetTailRightWingPtr(peb);
			peb->SetHeadRightWingPtr(pea);
		}
	}
	else if (pea->GetHeadVertexPtr() ==
		peb->GetTailVertexPtr())
	{
		if (pea->GetRightFacePtr() == this &&
			peb->GetRightFacePtr() == this)
		{
			pea->SetHeadRightWingPtr(peb);
			peb->SetTailRightWingPtr(pea);
		}
		else
		{
			pea->SetHeadLeftWingPtr(peb);
			peb->SetTailLeftWingPtr(pea);
		}
	}
	else
	{
		if (pea->GetRightFacePtr() == this &&
			peb->GetLeftFacePtr() == this)
		{
			pea->SetHeadRightWingPtr(peb);
			peb->SetHeadLeftWingPtr(pea);
		}
		else
		{
			pea->SetTailRightWingPtr(peb);
			peb->SetTailLeftWingPtr(pea);
		}
	}
}

// Join edge and kill vertex
BOOL WSurf::JoinEdgeKillVertex(WEdge *pea, WEdge *peb,
	WVert *pv)
{
	WEdge *phle;  // Head vertex left edge wing pointer
	WEdge *phre;  // Head vertex right edge wing pointer
	WFace *plf;   // Left face pointer
	WFace *prf;   // Right face pointer
	WVert *phv;   // Remaining head vertex pointer

	// Invert remaining edge (if necessary)
	if (pea->GetHeadVertexPtr() != pv)
		pea->Invert();

	// Invert discarded edge (if necessary)
	if (peb->GetTailVertexPtr() != pv)
		peb->Invert();

	// Get discarded head vertex pointer
	phv = peb->GetHeadVertexPtr();

	// Get discarded edge head vertex wing pointers
	phle = peb->GetHeadLeftWingPtr();
	phre = peb->GetHeadRightWingPtr();

	// Update remaining edge head vertex wing pointers
	pea->SetHeadLeftWingPtr(phle);
	pea->SetHeadRightWingPtr(phre);

	// Update head vertex left wing edge wing pointer
	if (phle->GetHeadVertexPtr() == pv)
		phle->SetHeadRightWingPtr(pea);
	else
		phle->SetTailLeftWingPtr(pea);

	// Update head vertex right wing edge wing pointer
	if (phre->GetHeadVertexPtr() == pv)
		phre->SetHeadLeftWingPtr(pea);
	else
		phre->SetTailRightWingPtr(pea);

	// Update remaining edge head vertex pointer
	pea->SetHeadVertexPtr(phv);

	// Get discarded edge face pointers
	plf = peb->GetLeftFacePtr();
	prf = peb->GetRightFacePtr();

	// Remove edge links from face edge link lists
	plf->RemoveLink(peb);
	prf->RemoveLink(peb);

	RemoveEdge(peb);      // Remove edge from edge list
	RemoveVertex(pv);     // Remove vertex from vertex list

	return TRUE;
}

// Kill edge and face
BOOL WSurf::KillEdgeFace(WEdge *pe, WFace *pf)
{
	WEdge *pce;   // Current edge pointer
	WEdge *phle;  // Head vertex left edge wing pointer
	WEdge *phre;  // Head vertex right edge wing pointer
	WEdge *ptle;  // Tail vertex left edge wing pointer
	WEdge *ptre;  // Tail vertex right edge wing pointer
	WLink *pcl;   // Current edge link pointer
	WLink *pnl;   // New edge link pointer
	WFace *prf;   // Remaining face pointer
	WVert *pv;    // Vertex pointer

	// Invert edge (if necessary)
	if (pf != pe->GetLeftFacePtr())
		pe->Invert();

	// Get remaining face pointer
	prf = pe->GetRightFacePtr();

	// Remove edge links from vertex edge link lists
	pe->GetHeadVertexPtr()->RemoveLink(pe);
	pe->GetTailVertexPtr()->RemoveLink(pe);

	// Remove edge link from face edge link list
	pf->RemoveLink(pe);

	// Get face edge link list pointer
	pcl = pf->GetLinkListPtr();

	do
	{
		pce = pcl->GetEdgePtr();    // Get current edge pointer

		// Relink edge face pointers
		if (pce->GetLeftFacePtr() == pf)
			pce->SetLeftFacePtr(prf);
		if (pce->GetRightFacePtr() == pf)
			pce->SetRightFacePtr(prf);

		if (pce->GetLeftFacePtr() != pce->GetRightFacePtr())
		{
			// Copy edge link
			if ((pnl = new WLink(pce)) == NULL)
				return FALSE;

			// Add new edge link to remaining face edge link list
			prf->AddLink(pnl);
		}

		pcl = pcl->GetNext();   // Get next edge link pointer
	} while (pcl != pf->GetLinkListPtr());

	// Get head vertex edge wing pointers
	phle = pe->GetHeadLeftWingPtr();
	phre = pe->GetHeadRightWingPtr();

	// Get head vertex pointer
	pv = pe->GetHeadVertexPtr();

	if (phle != phre)     // Separate edges ?
	{
		// Set edge wing pointers
		prf->SetWings(phle, phre);
	}
	else  // Single edge
	{
		// Set edge wing pointers
		if (phle->GetHeadVertexPtr() == pv)     // Head vertex ?
		{
			phle->SetHeadLeftWingPtr(NULL);
			phle->SetHeadRightWingPtr(NULL);
		}
		else    // Tail vertex
		{
			phle->SetTailLeftWingPtr(NULL);
			phle->SetTailRightWingPtr(NULL);
		}
	}

	// Get tail vertex edge wing pointers
	ptle = pe->GetTailLeftWingPtr();
	ptre = pe->GetTailRightWingPtr();

	// Get tail vertex pointer
	pv = pe->GetTailVertexPtr();

	if (ptle != ptre)     // Separate edges ?
	{
		// Set edge wing pointers
		prf->SetWings(ptle, ptre);
	}
	else  // Single edge
	{
		// Set edge wing pointers
		if (ptle->GetHeadVertexPtr() == pv)     // Head vertex ?
		{
			ptle->SetHeadLeftWingPtr(NULL);
			ptle->SetHeadRightWingPtr(NULL);
		}
		else    // Tail vertex
		{
			ptle->SetTailLeftWingPtr(NULL);
			ptle->SetTailRightWingPtr(NULL);
		}
	}

	// Remove edge link from remaining face edge link list
	prf->RemoveLink(pe);

	RemoveFace(pf);   // Remove face from face list
	RemoveEdge(pe);   // Remove edge from edge list

	return TRUE;
}

// Kill edge and vertex
BOOL WSurf::KillEdgeVertex(WEdge *pe, WVert *pv)
{
	WEdge *ptle;  // Tail vertex left edge wing pointer
	WEdge *ptre;  // Tail vertex right edge wing pointer
	WFace *pf;    // Face pointer
	WVert *prv;   // Remaining vertex pointer

	// Invert edge (if necessary)
	if (pv != pe->GetHeadVertexPtr())
		pe->Invert();

	// Get tail vertex edge wing pointers
	ptle = pe->GetTailLeftWingPtr();
	ptre = pe->GetTailRightWingPtr();

	// Get remaining vertex pointer
	prv = pe->GetOtherVertex(pv);

	// Get face pointer (both left and right faces are same)
	pf = pe->GetLeftFacePtr();

	// Relink tail vertex edge wing pointers
	if (ptle != NULL)
	{
		if (ptle != ptre)   // Separate edges ?
		{
			pf->SetWings(ptle, ptre);
		}
		else    // Single edge
		{
			if (ptle->GetHeadVertexPtr() == prv)  // Head vertex ?
			{
				ptle->SetHeadLeftWingPtr(NULL);
				ptle->SetHeadRightWingPtr(NULL);
			}
			else  // Tail vertex
			{
				ptle->SetTailLeftWingPtr(NULL);
				ptle->SetTailRightWingPtr(NULL);
			}
		}
	}

	// Remove edge link from face edge link list
	pf->RemoveLink(pe);

	// Remove edge link from vertex edge link list
	prv->RemoveLink(pe);

	RemoveEdge(pe);       // Remove edge from edge list
	RemoveVertex(pv);     // Remove vertex from vertex list

	return TRUE;
}

// Kill vertex and face
BOOL WSurf::KillVertexFace()
{
	RemoveVertex(pvlist); // Remove vertex from vertex list
	RemoveFace(pflist);   // Remove face from face list

	return TRUE;
}

// Make edge and face
BOOL WSurf::MakeEdgeFace(WVert *phv, WVert *ptv, WFace *pf,
	WEdge **ppe, WFace **ppf)
{
	WEdge *phle = nullptr;  // Head vertex left edge wing pointer
	WEdge *phre = nullptr;  // Head vertex right edge wing pointer
	WEdge *ptle = nullptr;  // Tail vertex left edge wing pointer
	WEdge *ptre = nullptr;  // Tail vertex right edge wing pointer
	WEdge *pce;   // Current edge pointer
	WEdge *pne;   // New edge pointer
	WFace *plf;   // Left face pointer
	WFace *pnf;   // New face pointer
	WFace *prf;   // Right face pointer
	WLink *pcl;   // Current edge link pointer
	WLink *pfl;   // First edge link pointer
	WLink *pnl;   // New edge link pointer
	WLink *pxl;   // Next edge link pointer
	WVert *pcv;   // Current vertex pointer

	// Get existing head vertex edge link list pointer
	pcl = phv->GetLinkListPtr();

	if (pcl != NULL)
	{
		do  // Find existing head vertex edge wings
		{
			pce = pcl->GetEdgePtr();  // Get current edge pointer

			if ((pce->GetHeadVertexPtr() == phv &&
				pce->GetRightFacePtr() == pf) ||
				(pce->GetTailVertexPtr() == phv &&
				pce->GetLeftFacePtr() == pf))
			{
				// Get head vertex left edge wing pointer
				phle = pce;

				// Get head vertex right edge wing pointer
				if (pce->GetHeadVertexPtr() == phv)
					phre = pce->GetHeadRightWingPtr();
				else
					phre = pce->GetTailLeftWingPtr();

				if (phre == NULL)       // Single edge ?
					phre = phle;

				break;
			}
			else if ((pce->GetHeadVertexPtr() == phv &&
				pce->GetLeftFacePtr() == pf) ||
				(pce->GetTailVertexPtr() == phv &&
				pce->GetRightFacePtr() == pf))
			{
				// Get head vertex right edge wing pointer
				phre = pce;

				// Get head vertex left edge wing pointer
				if (pce->GetHeadVertexPtr() == phv)
					phle = pce->GetHeadLeftWingPtr();
				else
					phle = pce->GetTailRightWingPtr();

				if (phle == NULL)       // Single edge ?
					phle = phre;

				break;
			}

			pcl = pcl->GetNext();     // Get next edge link
		} while (pcl != phv->GetLinkListPtr());
	}

	// Get existing tail vertex edge link list pointer
	pcl = ptv->GetLinkListPtr();

	if (pcl != NULL)
	{
		do  // Find existing tail vertex edge wings
		{
			pce = pcl->GetEdgePtr();  // Get current edge pointer

			if ((pce->GetHeadVertexPtr() == ptv &&
				pce->GetLeftFacePtr() == pf) ||
				(pce->GetTailVertexPtr() == ptv &&
				pce->GetRightFacePtr() == pf))
			{
				// Get tail vertex left edge wing pointer
				ptle = pce;

				// Get tail vertex right edge wing pointer
				if (pce->GetHeadVertexPtr() == ptv)
					ptre = pce->GetHeadLeftWingPtr();
				else
					ptre = pce->GetTailRightWingPtr();

				if (ptre == NULL)       // Single edge ?
					ptre = ptle;

				break;
			}
			else if ((pce->GetHeadVertexPtr() == ptv &&
				pce->GetRightFacePtr() == pf) ||
				(pce->GetTailVertexPtr() == ptv &&
				pce->GetLeftFacePtr() == pf))
			{
				// Get tail vertex right edge wing pointer
				ptre = pce;

				// Get tail vertex left edge wing pointer
				if (pce->GetHeadVertexPtr() == ptv)
					ptle = pce->GetHeadRightWingPtr();
				else
					ptle = pce->GetTailLeftWingPtr();

				if (ptle == NULL)       // Single edge ?
					ptle = ptre;

				break;
			}

			pcl = pcl->GetNext();     // Get next edge link
		} while (pcl != ptv->GetLinkListPtr());
	}

	// Allocate new edge
	if ((pne = new WEdge) == NULL)
		return FALSE;

	// Allocate new face
	if ((pnf = new WFace) == NULL)
	{
		delete pne;         // Delete new edge
		return FALSE;
	}

	// Add edge link to head vertex edge link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		delete pne;         // Delete new edge
		delete pnf;         // Delete new face
		return FALSE;
	}
	else
		phv->AddLink(pnl);

	// Add edge link to tail vertex edge link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		// Remove edge link from vertex edge link list
		phv->RemoveLink(pne);

		delete pne;         // Delete new edge
		delete pnf;         // Delete new face
		return FALSE;
	}
	else
		ptv->AddLink(pnl);

	// Add edge link to existing face edge link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		// Remove edge links from vertex edge link list
		phv->RemoveLink(pne);
		ptv->RemoveLink(pne);

		delete pne;         // Delete new edge
		delete pnf;         // Delete new face
		return FALSE;
	}
	else
		pf->AddLink(pnl);

	// Add edge link to new face edge link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		// Remove edge links from vertex edge link list
		phv->RemoveLink(pne);
		ptv->RemoveLink(pne);

		// Remove edge link from existing face edge link list
		pf->RemoveLink(pne);

		delete pne;         // Delete new edge
		delete pnf;         // Delete new face
		return FALSE;
	}
	else
		pnf->AddLink(pnl);

	AddEdge(pne);     // Insert new edge into edge list
	AddFace(pnf);     // Insert new face into face list

	// Link vertices to new edge
	pne->SetHeadVertexPtr(phv);
	pne->SetTailVertexPtr(ptv);

	// Link faces to new edge
	pne->SetLeftFacePtr(pnf);
	pne->SetRightFacePtr(pf);

	// Scan edges clockwise from head vertex
	pcv = phv;
	pce = phle;
	for (;;)
	{
		if (pce->GetLeftFacePtr() != pce->GetRightFacePtr())
		{
			// Remove edge link from existing face edge link list
			pf->RemoveLink(pce);
		}

		// Allocate new edge link
		if ((pnl = new WLink(pce)) == NULL)
		{
			// Delete new face edge link list
			if ((pfl = pnf->GetLinkListPtr()) != NULL)
			{
				pcl = pfl;
				do
				{
					// Get current edge pointer
					pce = pcl->GetEdgePtr();

					// Get current edge face pointers
					plf = pce->GetLeftFacePtr();
					prf = pce->GetRightFacePtr();

					// Restore edge face pointers
					if (plf == pnf)
						pce->SetLeftFacePtr(pf);
					if (prf == pnf)
						pce->SetRightFacePtr(pf);

					pxl = pcl->GetNext();

					if (plf != prf)
					{
						// Add edge link to existing face edge link list
						pf->AddLink(pcl);
					}
					else
						delete pcl;

					pcl = pxl;
				} while (pcl != pfl);
			}

			// Remove edge links from vertex edge link list
			phv->RemoveLink(pne);
			ptv->RemoveLink(pne);

			// Remove edge link from existing face edge link list
			pf->RemoveLink(pne);

			delete pne;       // Delete new edge
			delete pnf;       // Delete new face
			return FALSE;
		}

		// Add edge link to new face edge link list
		pnf->AddLink(pnl);

		if (pcv == pce->GetTailVertexPtr())     // Tail vertex ?
		{
			// Replace existing face with new face
			pce->SetLeftFacePtr(pnf);

			// Update current vertex pointer
			pcv = pce->GetHeadVertexPtr();

			// Get next edge pointer
			if (pce != ptle)
				pce = pce->GetHeadLeftWingPtr();
			else
				break;
		}
		else        // Head vertex
		{
			// Replace existing face with new face
			pce->SetRightFacePtr(pnf);

			// Update current vertex pointer
			pcv = pce->GetTailVertexPtr();

			// Get next edge pointer
			if (pce != ptle)
				pce = pce->GetTailRightWingPtr();
			else
				break;
		}
	}

	// Set edge wing pointers
	pnf->SetWings(pne, phle);
	pf->SetWings(pne, phre);
	pnf->SetWings(pne, ptle);
	pf->SetWings(pne, ptre);

	*ppe = pne;   // Set new edge pointer
	*ppf = pnf;   // Set new face pointer

	return TRUE;
}

// Make edge and vertex
BOOL WSurf::MakeEdgeVertex(WFace *pf, WVert *pv, point3
	&posn, WEdge **ppe, WVert **ppv)
{
	WEdge *pce;           // Current edge pointer
	WEdge *pne;           // New edge pointer
	WEdge *ptle = NULL;   // Tail vertex left edge wing ptr
	WEdge *ptre = NULL;   // Tail vertex right edge wing ptr
	WLink *pcl;           // Current link pointer
	WLink *pnl;           // New edge link pointer
	WVert *pnv;           // New vertex pointer

	// Get existing vertex edge link list pointer
	pcl = pv->GetLinkListPtr();

	if (pcl != NULL)
	{
		do  // Find existing tail vertex edge wings
		{
			pce = pcl->GetEdgePtr();  // Get current edge pointer

			if ((pce->GetHeadVertexPtr() == pv &&
				pce->GetLeftFacePtr() == pf) ||
				(pce->GetTailVertexPtr() == pv &&
				pce->GetRightFacePtr() == pf))
			{
				// Get tail vertex left edge wing pointer
				ptle = pce;

				// Get tail vertex right edge wing pointer
				if (pce->GetHeadVertexPtr() == pv)
					ptre = pce->GetHeadLeftWingPtr();
				else
					ptre = pce->GetTailRightWingPtr();

				if (ptre == NULL)       // Single edge ?
					ptre = ptle;

				break;
			}
			else if ((pce->GetHeadVertexPtr() == pv &&
				pce->GetRightFacePtr() == pf) ||
				(pce->GetTailVertexPtr() == pv &&
				pce->GetLeftFacePtr() == pf))
			{
				// Get tail vertex right edge wing pointer
				ptre = pce;

				// Get tail vertex left edge wing pointer
				if (pce->GetHeadVertexPtr() == pv)
					ptle = pce->GetHeadRightWingPtr();
				else
					ptle = pce->GetTailLeftWingPtr();

				if (ptle == NULL)       // Single edge ?
					ptle = ptre;

				break;
			}

			pcl = pcl->GetNext();     // Get next edge link
		} while (pcl != pv->GetLinkListPtr());
	}

	// Allocate new edge
	if ((pne = new WEdge) == NULL)
		return FALSE;

	// Allocate new vertex
	if ((pnv = new WVert(posn)) == NULL)
	{
		delete pne;
		return FALSE;
	}

	// Add edge link to face edge link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		delete pne;
		delete pnv;
		return FALSE;
	}
	else
		pf->AddLink(pnl);

	// Add edge link to existing vertex edge link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		// Remove edge link from vertex edge link list
		pf->RemoveLink(pne);

		delete pne;         // Delete new edge
		delete pnv;         // Delete new vertex
		return FALSE;
	}
	else
		pv->AddLink(pnl);

	// Add edge link to new vertex edge link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		// Remove edge links from vertex edge link list
		pf->RemoveLink(pne);
		pv->RemoveLink(pne);

		delete pne;         // Delete new edge
		delete pnv;         // Delete new vertex
		return FALSE;
	}
	else
		pnv->AddLink(pnl);

	AddEdge(pne);     // Insert new edge into edge list
	AddVertex(pnv);   // Insert new vertex into vertex list

	// Link face to new edge
	pne->SetLeftFacePtr(pf);
	pne->SetRightFacePtr(pf);

	// Link vertices to new edge
	pne->SetHeadVertexPtr(pnv);
	pne->SetTailVertexPtr(pv);

	// Set tail vertex edge wing pointers
	if (ptle != NULL)
	{
		if (ptle != ptre)   // Separate edges ?
		{
			pf->SetWings(pne, ptle);
			pf->SetWings(pne, ptre);
		}
		else    // Single edge
		{
			pne->SetTailLeftWingPtr(ptle);
			pne->SetTailRightWingPtr(ptle);

			if (ptle->GetHeadVertexPtr() == pv)   // Head vertex ?
			{
				ptle->SetHeadLeftWingPtr(pne);
				ptle->SetHeadRightWingPtr(pne);
			}
			else  // Tail vertex
			{
				ptle->SetTailLeftWingPtr(pne);
				ptle->SetTailRightWingPtr(pne);
			}
		}
	}

	*ppe = pne;   // Set new edge pointer
	*ppv = pnv;   // Set new vertex pointer

	return TRUE;
}

// Make vertex and face
BOOL WSurf::MakeVertexFace(point3 &posn, WVert **ppv,
	WFace **ppf)
{
	WFace *pnf;   // New face pointer
	WVert *pnv;   // New vertex pointer

	// Allocate new vertex
	if ((pnv = new WVert(posn)) == NULL)
		return FALSE;

	// Allocate new face
	if ((pnf = new WFace) == NULL)
	{
		delete pnv;
		return FALSE;
	}

	AddVertex(pnv);   // Insert new vertex into vertex list
	AddFace(pnf);     // Insert new face into face list

	*ppf = pnf;       // Set new face pointer
	*ppv = pnv;       // Set new vertex pointer

	return TRUE;
}

// Split edge and make vertex
BOOL WSurf::SplitEdgeMakeVertex(WEdge *pe, point3 &posn,
	WEdge **ppe, WVert **ppv)
{
	WEdge *pne;   // New edge pointer
	WFace *plf;   // Left face pointer
	WFace *prf;   // Right face pointer
	WLink *pnl;   // New edge link pointer
	WVert *pnv;   // New vertex pointer

	// Allocate new edge
	if ((pne = new WEdge) == NULL)
		return FALSE;

	// Allocate new vertex
	if ((pnv = new WVert(posn)) == NULL)
	{
		delete pne;         // Delete new edge
		return FALSE;
	}

	// Add existing edge link to new vertex edge link list
	if ((pnl = new WLink(pe)) == NULL)
	{
		delete pne;         // Delete new edge
		delete pnv;         // Delete new vertex
		return FALSE;
	}
	else
		pnv->AddLink(pnl);

	// Add new edge link to new vertex edge link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		// Remove existing edge link from list
		pnv->RemoveLink(pe);

		delete pne;         // Delete new edge
		delete pnv;         // Delete new vertex
		return FALSE;
	}
	else
		pnv->AddLink(pnl);

	// Get face pointers
	plf = pe->GetLeftFacePtr();
	prf = pe->GetRightFacePtr();

	// Insert new edge link into left face link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		// Remove edge links from vertex edge link list
		pnv->RemoveLink(pe);
		pnv->RemoveLink(pne);

		delete pne;         // Delete new edge
		delete pnv;         // Delete new vertex
		return FALSE;
	}
	else
		plf->AddLink(pnl);

	// Insert new edge link into right face link list
	if ((pnl = new WLink(pne)) == NULL)
	{
		// Remove edge links from vertex edge link list
		pnv->RemoveLink(pe);
		pnv->RemoveLink(pne);

		// Remove edge link from left face edge link list
		plf->RemoveLink(pe);

		delete pne;         // Delete new edge
		delete pnv;         // Delete new vertex
		return FALSE;
	}
	else
		prf->AddLink(pnl);

	// Initialize new edge vertex pointers
	pne->SetHeadVertexPtr(pe->GetHeadVertexPtr());
	pne->SetTailVertexPtr(pnv);

	// Initialize new edge face pointers
	pne->SetLeftFacePtr(plf);
	pne->SetRightFacePtr(prf);

	// Update new edge wing pointers
	pne->SetHeadLeftWingPtr(pe->GetHeadLeftWingPtr());
	pne->SetHeadRightWingPtr(pe->GetHeadRightWingPtr());
	pne->SetTailLeftWingPtr(pe);
	pne->SetTailRightWingPtr(pe);

	AddEdge(pne);     // Insert new edge into edge list
	AddVertex(pnv);   // Insert new vertex into vertex list

	// Update existing edge head vertex pointer
	pe->SetHeadVertexPtr(pnv);

	// Update existing edge wing pointers
	pe->SetHeadLeftWingPtr(pne);
	pe->SetHeadRightWingPtr(pne);

	*ppe = pne;   // Set new edge pointer
	*ppv = pnv;   // Set new vertex pointer

	return TRUE;
}

// Get next edge around vertex
WEdge *WVert::GetNextEdge(WEdge *pe, BOOL cw_dir)
{
	WEdge *pne;   // Next edge pointer

	if (cw_dir == TRUE)   // Clockwise direction ?
	{
		if (pe->GetHeadVertexPtr() == this)
			pne = pe->GetHeadLeftWingPtr();
		else
			pne = pe->GetTailRightWingPtr();
	}
	else  // Counterclockwise direction
	{
		if (pe->GetHeadVertexPtr() == this)
			pne = pe->GetHeadRightWingPtr();
		else
			pne = pe->GetTailLeftWingPtr();
	}

	return pne;
}

// Get next face around vertex
WFace *WVert::GetNextFace(WFace *pf, BOOL cw_dir)
{
	BOOL head_flag;   // Head vertex flag
	WEdge *pe;        // Edge pointer
	WLink *pl;        // Edge link pointer

	// Get face edge link list pointer
	pl = pf->GetLinkListPtr();

	do
	{
		pe = pl->GetEdgePtr();      // Get edge pointer

		// Determine whether current edge contains vertex
		if (pe->GetHeadVertexPtr() == this)
		{
			head_flag = TRUE;
			break;
		}
		else if (pe->GetTailVertexPtr() == this)
		{
			head_flag = FALSE;
			break;
		}

		pl = pl->GetNext();     // Get next edge link
	} while (pl != pf->GetLinkListPtr());

	if (cw_dir == TRUE)   // Clockwise direction ?
	{
		if (pf == pe->GetLeftFacePtr())
		{
			if (head_flag == TRUE)
				pe = pe->GetHeadLeftWingPtr();
		}
		else
		{
			if (head_flag == FALSE)
				pe = pe->GetTailRightWingPtr();
		}
	}
	else  // Counterclockwise direction
	{
		if (pf == pe->GetLeftFacePtr())
		{
			if (head_flag == FALSE)
				pe = pe->GetHeadRightWingPtr();
		}
		else
		{
			if (head_flag == TRUE)
				pe = pe->GetTailLeftWingPtr();
		}
	}

	return pe->GetOtherFace(pf);
}