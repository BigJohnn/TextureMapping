////////////////////////////////////////////////////////////
//
//  WINGED.H - Winged Edge Boundary Representation Class
//             Include File
//
//  Version:    1.00B
//
//  History:    96/05/05 - Version 1.00A release.
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

#ifndef _WINGED_H
#define _WINGED_H
#include "GENERAL.h"

// Forward references
class WFace;
class WVert;

class WEdge             // Winged edge
{
private:
	WEdge *pnext;       // Next edge pointer
	WEdge *pprev;       // Previous edge pointer
	WEdge *phle;        // Head vertex left face edge ptr
	WEdge *phre;        // Head vertex right face edge ptr
	WEdge *ptle;        // Tail vertex left face edge ptr
	WEdge *ptre;        // Tail vertex right face edge ptr
	WFace *plf;         // Left face pointer
	WFace *prf;         // Right face pointer
	WVert *phv;         // Head vertex pointer
	WVert *ptv;         // Tail vertex pointer
public:
	double Esmooth;

public:
	WEdge()
	{
		phle = phre = ptle = ptre = NULL;
		plf = prf = NULL;
		phv = ptv = NULL;
		pnext = pprev = this;
	}

	void SetHeadLeftWingPtr(WEdge *pe) { phle = pe; }
	void SetHeadRightWingPtr(WEdge *pe) { phre = pe; }
	void SetHeadVertexPtr(WVert *pv) { phv = pv; }
	void SetLeftFacePtr(WFace *pf) { plf = pf; }
	void SetRightFacePtr(WFace *pf) { prf = pf; }
	void SetTailLeftWingPtr(WEdge *pe) { ptle = pe; }
	void SetTailRightWingPtr(WEdge *pe) { ptre = pe; }
	void SetTailVertexPtr(WVert *pv) { ptv = pv; }
	WEdge *GetHeadLeftWingPtr() { return phle; }
	WEdge *GetHeadRightWingPtr() { return phre; }
	WEdge *GetNext() { return pnext; }
	WEdge *GetPrev() { return pprev; }
	WEdge *GetTailLeftWingPtr() { return ptle; }
	WEdge *GetTailRightWingPtr() { return ptre; }
	WFace *GetLeftFacePtr() { return plf; }
	WFace *GetOtherFace(WFace *pf)
	{
		return ((pf == plf) ? prf : plf);
	}
	WFace *GetRightFacePtr() { return prf; }
	WVert *GetHeadVertexPtr() { return phv; }
	WVert *GetOtherVertex(WVert *pv)
	{
		return ((pv == phv) ? ptv : phv);
	}
	WVert *GetTailVertexPtr() { return ptv; }

	void Invert()
	{
		WEdge *pce;   // Temporary edge pointer
		WFace *pcf;   // Temporary face pointer
		WVert *pcv;   // Temporary vertex pointer

		// Invert faces
		pcf = plf;
		plf = prf;
		prf = pcf;

		// Invert vertices
		pcv = phv;
		phv = ptv;
		ptv = pcv;

		// Invert edge wing pointers
		pce = phle;
		phle = ptre;
		ptre = pce;
		pce = phre;
		phre = ptle;
		ptle = pce;
	}

	void SetNext(WEdge *pn) { pnext = pn; }
	void SetPrev(WEdge *pp) { pprev = pp; }
	void Terminate(WVert *pv)
	{
		if (pv == phv)
			phle = phre = NULL;
		else
			ptle = ptre = NULL;
	}
};

class WLink             // Edge link
{
private:
	WLink *pnext;       // Next edge link pointer
	WLink *pprev;       // Previous edge link pointer
	WEdge *pedge;       // Edge pointer

public:
	WLink(WEdge *pe = NULL)
	{
		pedge = pe;
		pnext = pprev = this;
	}

	WEdge *GetEdgePtr() { return pedge; }
	WLink *GetNext() { return pnext; }
	WLink *GetPrev() { return pprev; }
	void SetNext(WLink *pn) { pnext = pn; }
	void SetPrev(WLink *pp) { pprev = pp; }
};

class WVert             // Vertex
{
private:
	WVert *pnext;       // Next vertex pointer
	WVert *pprev;       // Previous vertex pointer
	point3 posn;        // Coordinates
	WLink *pllist;      // Edge link list pointer

public:
	WVert(point3 &p)
	{
		posn = p;
		pllist = NULL;
		pnext = pprev = this;
	}

	~WVert()
	{
		WLink *pcl = nullptr;       // Current edge link pointer
		WLink *pnl = nullptr;       // Next edge link pointer

		// Delete edge link list
		if (pllist != NULL)
		{
			pcl = pllist;
			do
			{
				pnl = pcl->GetNext();
				delete pcl;
				pcl = pnl;
			} while (pcl != pllist);
		}
	}

	point3 &GetPosn() { return posn; }
	WEdge *GetNextEdge(WEdge *, BOOL);
	WFace *GetNextFace(WFace *, BOOL);
	WLink *GetLinkListPtr() { return pllist; }

	// Remove link from link list
	void RemoveLink(WEdge *pe)
	{
		BOOL found = FALSE;       // Edge link found flag
		WLink *pl;                // Link pointer

		// Find edge link
		pl = pllist;
		do
		{
			if (pl->GetEdgePtr() == pe)
			{
				found = TRUE;
				break;
			}
			pl = pl->GetNext();
		} while (pl != pllist);

		if (found == FALSE)
			return;

		pl->GetPrev()->SetNext(pl->GetNext());
		pl->GetNext()->SetPrev(pl->GetPrev());

		if (pl->GetNext() != pl)
			pllist = pl->GetNext();
		else
			pllist = NULL;

		delete pl;
	}

	WVert *GetNext() { return pnext; }
	WVert *GetPrev() { return pprev; }

	void AddLink(WLink *pl)   // Add link to link list
	{
		if (pllist != NULL)
		{
			pl->SetNext(pllist);
			pl->SetPrev(pllist->GetPrev());
			pllist->GetPrev()->SetNext(pl);
			pllist->SetPrev(pl);
		}
		pllist = pl;
	}

	void SetLinkListPtr(WLink *pl) { pllist = pl; }
	void SetPosn(point3 &p) { posn = p; }
	void SetNext(WVert *pn) { pnext = pn; }
	void SetPrev(WVert *pp) { pprev = pp; }
};

class WFace             // Face
{
private:
	WFace *pnext;       // Next face pointer
	WFace *pprev;       // Previous face pointer
	WLink *pllist;      // Edge link list pointer
public:
	double Edata;

public:
	WFace()
	{
		pllist = NULL;
		pnext = pprev = this;
	}

	~WFace()
	{
		WLink *pcl = nullptr;       // Current edge link pointer
		WLink *pnl = nullptr;       // Next edge link pointer

		// Delete edge link list
		if (pllist != NULL)
		{
			pcl = pllist;
			do
			{
				pnl = pcl->GetNext();
				delete pcl;
				pcl = pnl;
			} while (pcl != pllist);
		}
	}

	WEdge *GetNextEdge(WEdge *, BOOL);
	WFace *GetNext() { return pnext; }
	WFace *GetPrev() { return pprev; }
	WLink *GetLinkListPtr() { return pllist; }

	// Remove link from link list
	void RemoveLink(WEdge *pe)
	{
		BOOL found = FALSE;       // Edge link found flag
		WLink *pl;                // Link pointer

		// Find edge link
		pl = pllist;
		do
		{
			if (pl->GetEdgePtr() == pe)
			{
				found = TRUE;
				break;
			}
			pl = pl->GetNext();
		} while (pl != pllist);

		if (found == FALSE)
			return;

		pl->GetPrev()->SetNext(pl->GetNext());
		pl->GetNext()->SetPrev(pl->GetPrev());

		if (pl->GetNext() != pl)
			pllist = pl->GetNext();
		else
			pllist = NULL;

		delete pl;
	}

	void AddLink(WLink *pl)   // Add link to link list
	{
		if (pllist != NULL)
		{
			pl->SetNext(pllist);
			pl->SetPrev(pllist->GetPrev());
			pllist->GetPrev()->SetNext(pl);
			pllist->SetPrev(pl);
		}
		pllist = pl;
	}

	void SetLinkListPtr(WLink *pl) { pllist = pl; }
	void SetNext(WFace *pn) { pnext = pn; }
	void SetPrev(WFace *pp) { pprev = pp; }
	void SetWings(WEdge *, WEdge *);
};

class WSurf             // Surface
{
private:
	WEdge *pelist;      // Edge list pointer
	WFace *pflist;      // Face list pointer
	WVert *pvlist;      // Vertex list pointer
	WSurf *pnext;       // Next surface pointer
	WSurf *pprev;       // Previous surface pointer

	void AddEdge(WEdge *pe)   // Add edge to edge list
	{
		if (pelist != NULL)
		{
			pe->SetNext(pelist);
			pe->SetPrev(pelist->GetPrev());
			pelist->GetPrev()->SetNext(pe);
			pelist->SetPrev(pe);
		}
		pelist = pe;
	}

	void AddFace(WFace *pf)   // Add face to face list
	{
		if (pflist != NULL)
		{
			pf->SetNext(pflist);
			pf->SetPrev(pflist->GetPrev());
			pflist->GetPrev()->SetNext(pf);
			pflist->SetPrev(pf);
		}
		pflist = pf;
	}

	// Add vertex to vertex list
	void AddVertex(WVert *pv)
	{
		if (pvlist != NULL)
		{
			pv->SetNext(pvlist);
			pv->SetPrev(pvlist->GetPrev());
			pvlist->GetPrev()->SetNext(pv);
			pvlist->SetPrev(pv);
		}
		pvlist = pv;
	}

	// Remove edge from edge list
	void RemoveEdge(WEdge *pe)
	{
		pe->GetPrev()->SetNext(pe->GetNext());
		pe->GetNext()->SetPrev(pe->GetPrev());

		if (pe->GetNext() != pe)
			pelist = pe->GetNext();
		else
			pelist = NULL;

		delete pe;
	}

	// Remove face from face list
	void RemoveFace(WFace *pf)
	{
		pf->GetPrev()->SetNext(pf->GetNext());
		pf->GetNext()->SetPrev(pf->GetPrev());

		if (pf->GetNext() != pf)
			pflist = pf->GetNext();
		else
			pflist = NULL;

		delete pf;
	}

	// Remove vertex from vertex list
	void RemoveVertex(WVert *pv)
	{
		pv->GetPrev()->SetNext(pv->GetNext());
		pv->GetNext()->SetPrev(pv->GetPrev());

		if (pv->GetNext() != pv)
			pvlist = pv->GetNext();
		else
			pvlist = NULL;

		delete pv;
	}

	void SetEdgeListPtr(WEdge *pe) { pelist = pe; }
	void SetFaceListPtr(WFace *pf) { pflist = pf; }
	void SetVertListPtr(WVert *pv) { pvlist = pv; }
	void SetNext(WSurf *pn) { pnext = pn; }
	void SetPrev(WSurf *pp) { pprev = pp; }

public:
	WSurf()
	{
		pelist = NULL;
		pflist = NULL;
		pvlist = NULL;
		pnext = pprev = this;
	}

	~WSurf()
	{
		WEdge *pce;   // Current edge pointer
		WEdge *pne;   // Next edge pointer
		WFace *pcf;   // Current face pointer
		WFace *pnf;   // Next face pointer
		WVert *pcv;   // Current vertex pointer
		WVert *pnv;   // Next vertex pointer

		// Delete edge list
		if (pelist != NULL)
		{
			pce = pelist;
			do
			{
				pne = pce->GetNext();
				delete pce;
				pce = pne;
			} while (pce != pelist);
		}

		// Delete face list
		if (pflist != NULL)
		{
			pcf = pflist;
			do
			{
				pnf = pcf->GetNext();
				delete pcf;
				pcf = pnf;
			} while (pcf != pflist);
		}

		// Delete vertex list
		if (pvlist != NULL)
		{
			pcv = pvlist;
			do
			{
				pnv = pcv->GetNext();
				delete pcv;
				pcv = pnv;
			} while (pcv != pvlist);
		}
	}

	BOOL JoinEdgeKillVertex(WEdge *, WEdge *, WVert *);
	BOOL KillEdgeFace(WEdge *, WFace *);
	BOOL KillEdgeVertex(WEdge *, WVert *);
	BOOL KillVertexFace();
	BOOL MakeEdgeFace(WVert *, WVert *, WFace *, WEdge **,
		WFace **);
	BOOL MakeEdgeVertex(WFace *, WVert *, point3 &,
		WEdge **, WVert **);
	BOOL MakeVertexFace(point3 &, WVert **, WFace **);
	BOOL SplitEdgeMakeVertex(WEdge *, point3 &, WEdge **,
		WVert **);
	WEdge *GetEdgeListPtr() { return pelist; }
	WFace *GetFaceListPtr() { return pflist; }
	WVert *GetVertListPtr() { return pvlist; }
	WSurf *GetNext() { return pnext; }
	WSurf *GetPrev() { return pprev; }
};
#endif