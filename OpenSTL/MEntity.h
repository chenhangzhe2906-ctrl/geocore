#pragma once
#include "pch.h"
#include "MObject.h"
#include "MBBox.h"

class MLine;

#define TC_ID_INVALID (-1)

class MEntity : public MObject
{
public:
	MEntity(void);
	MEntity(const MEntity& entity);
	virtual ~MEntity(void);
	//TC_OBJECT_TYPE2(TCEntity, ENTITY_OBJECT, TCObject);

public:
	MEntity& operator =(const MEntity& entity);

public:
	const MBBox& GetBBox() const { return m_bbox; }
	void SetBBox(const MBBox& bbox) { m_bbox = bbox; }
	int GetID() const;
	void SetID(int nID);
	bool HitTest(const MLine& cursorRay) const;
	bool HitTest(const MLine& cursorRay, const MMatrix& matrix) const;

public:
	virtual void CalculateBBox() {}
	virtual void Write(ostream& os, int verTag) const;
	virtual bool Read(ifstream& is, int verTag);

public:
	static int GetNextID();
	static void SetNextID(int nID);

protected:
	int m_nID;
	MBBox m_bbox;
	static int m_nNextID;           // id∑÷≈‰
};

class MEntityAnonymous : public MEntity
{
public:
	MEntityAnonymous(void) {};
	virtual ~MEntityAnonymous(void) {};
	//TC_OBJECT_TYPE2(MEntityAnonymous, ENTITY_ANONYMOUS_OBJECT, MEntity);
};