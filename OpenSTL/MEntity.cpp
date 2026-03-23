#include "pch.h"
#include "MEntity.h"
#include "MMatrix.h"
#include "MLine.h"

#define TC_ENTITY_ID_NORMAL_START 100
int MEntity::m_nNextID = TC_ENTITY_ID_NORMAL_START;

int MEntity::GetNextID()
{
	return m_nNextID++;
}

void MEntity::SetNextID(int nID)
{
	m_nNextID = nID;
	if (m_nNextID < TC_ENTITY_ID_NORMAL_START)
	{
		m_nNextID = TC_ENTITY_ID_NORMAL_START;
	}
}

MEntity::MEntity(void)
{
	m_nID = GetNextID();
}

MEntity::MEntity(const MEntity& entity)
	: MObject(entity)
{
	m_nID = entity.GetID();
	m_bbox = entity.m_bbox;
}

MEntity& MEntity::operator =(const MEntity& entity)
{
	if (this == &entity)
		return *this;

	MObject::operator =(entity);
	m_nID = entity.m_nID;
	m_bbox = entity.m_bbox;

	return *this;
}

MEntity::~MEntity(void)
{
}

void MEntity::Write(ostream& os, int verTag) const
{
	__super::Write(os, verTag);

	m_bbox.Write(os);
	os.write((const char*)&m_nID, sizeof(int));
}

bool MEntity::Read(ifstream& is, int verTag)
{
	bool bSuc = __super::Read(is, verTag);
	if (!bSuc)
	{
		return false;
	}

	m_bbox.Read(is);
	is.read((char*)&m_nID, sizeof(int));

	return true;
}

int MEntity::GetID() const
{
	return m_nID;
}

void MEntity::SetID(int nID)
{
	m_nID = nID;
}

bool MEntity::HitTest(const MLine& cursorRay) const
{
	return !m_bbox.IsDisjoint(cursorRay);
}

bool MEntity::HitTest(const MLine& cursorRay, const MMatrix& matrix) const
{
	MMatrix tcMat(matrix);
	if (tcMat.Inverse())
	{
		MLine line(tcMat * cursorRay.Point(), tcMat * cursorRay.Direction());
		return !m_bbox.IsDisjoint(line);
	}
	else
		return !m_bbox.IsDisjoint(cursorRay);
}
