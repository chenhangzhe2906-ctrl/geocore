#include "pch.h"
#include "MObject.h"

MObject::MObject(void)
{
	m_state = 0;
	SetVisible(TRUE);
	SetEnable(TRUE);
}

MObject::MObject(const MObject& object)
{
	m_state = object.m_state;
}

MObject::~MObject(void)
{
}

inline MObject& MObject::operator=(const MObject& object)
{
	if (&object == this) {
		return *this;
	}
	else {
		m_state = object.m_state;
		return *this;
	}
}



inline void MObject::Write(ostream& os, int verTag) const
{
	os.write((const char*)&m_state, sizeof(UINT));

}

bool MObject::Read(ifstream& is, int verTag)
{
	if (!is.is_open()) {
		return false;
	}
	is.read((char*)&m_state, sizeof(UINT));
	return true;
}

