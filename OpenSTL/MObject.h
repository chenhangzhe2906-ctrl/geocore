#pragma once
#include"pch.h"

/*

| 是按位或
作用：把 TC_ENTITY_SELECTED 那一位设置为 1,其它位保持不变

~ 是按位取反
~TC_ENTITY_SELECTED 把这个标志位变成 0，其它位变成 1

& 是按位与
作用：把选中那一位清零，其它位保持不变

*/

enum ObjectType_e
{
	NONE_OBJECT = 0x00000000,
	SUPER_OBJECT = 0x00000001,
	ENTITY_OBJECT = 0x00000002,
	MESH_OBJECT = 0x00000004,
	VERTEX_OBJECT = 0x00000008,

	EDGE_OBJECT = 0x00000010,
	FACE_OBJECT = 0x00000020,
	JAW_OBJECT = 0x00000040,
	TOOTH_OBJECT = 0x00000080,

	GUM_OBJECT = 0x00000100,
	JAWLINE_OBJECT = 0x00000200,
	TOOTHLIST_OBJECT = 0x00000400,
	JAWBONE_OBJECT = 0x00000800,
	HINGEAXIS_OBJECT = 0x00000801,
	COLORMESH_OBJECT = 0x00000802,
	SCAN_FACE_OBJECT = 0x00000804,

	AUX_OBJECT = 0x00001000,
	AUX_POINT_OBJECT = 0x00002000,
	AUX_ARROW_OBJECT = 0x00004000,
	AUX_FACE_OBJECT = 0x00008000,

	AUX_FRAME_OBJECT = 0x00010000,
	AUX_PLANE_OBJECT = 0x00020000,
	AUX_POLYLINE_OBJECT = 0x00040000,
	AUX_TORUS_OBJECT = 0x00080000,

	AUX_CIRCLE_OBJECT = 0x00100000,
	AUX_LINE_OBJECT = 0x00200000,
	AUX_TEXT_OBJECT = 0x00400000,
	AUX_PIPELINE_OBJECT = 0x00800000,

	AUX_BITMAP_OBJECT = 0x01000000,
	AUX_BILLBOARD_OBJECT = 0x02000000,
	AUX_2DLINE_OBJECT = 0x04000000,
	AUX_TEXTURE_OBJECT = 0x08000000,

	ATTACH_OBJECT = 0x10000000,
	ROOT_OBJECT = 0x10000001,
	POINTSET_OBJECT = 0x10000002,
	ATTACHCZ_OBJECT = 0x10000004,
	ATTACHSP_OBJECT = 0x10000008,

	ATTACHSJ_OBJECT = 0x10000010,
	ATTACHSD_OBJECT = 0x10000020,
	ATTACHPD_OBJECT = 0x10000040,
	ATTACHNZ_OBJECT = 0x10000080,

	ATTACHSPP_OBJECT = 0x10000100,
	ATTACHSPA_OBJECT = 0x10000200,
	ATTACH_LOWER_DEV_OBJECT = 0x10000800,
	ATTACH_CUTTING_INSTR = 0x10001000,
	ATTACH_LINGUAL_BUTTON = 0x10002000,
	ATTACH_TOWING_HOOK = 0x10004000,

	AUX_BOX_OBJECT = 0x10001000,
	AUX_MARKER_OBJECT = 0x10002000,
	AUX_GRID_OBJECT = 0x10004000,
	AUX_2DDEGREE_OBJECT = 0x10008000,

	AUX_NOTATION_OBJECT = 0x10010000,
	AUX_TEXT3D_OBJECT = 0x10020000,
	AUX_PLANE2D_OBJECT = 0x10040000,
	ENTITY_ANONYMOUS_OBJECT = 0x10080000,

	AUX_RAINBOW_OBJECT = 0x10100000,
	AUX_GRIDSCALE_OBJECT = 0x10200000,
	AUX_TRANSFORM_OBJECT = 0x10400000,
	AUX_BUTTON_OBJECT = 0x10800000,

	AUX_CROSS_OBJECT = 0x11000000,
	AUX_TORUS_2D_OBJECT = 0x12000000,
	AUX_RECTTANGLE_BOX_OBJECT = 0x14000000,
	CBCT_OBJECT = 0x18000000,

	AUX_SUPER_PLANE_OBJECT = 0x20000000,
	BRACKET_OBJECT = 0x20000001,
	AUX_LINE2D_OBJECT = 0x20000002,
	AUX_MESH_OBJECT = 0x20000004,
	AUX_COLORTABLE_OBJECT = 0x20000008,
	AIRWAY_OBJECT = 0x20001000,
	TOOTH_EMBEDED_OBJECT = 0x40000000,
	GLASSES_OBJECT = 0x40000001,
	GLASSES_LENSES_OBJECT = 0x40000002,
	GLASSES_FOOT_OBJECT = 0x40000004,
	GLASSES_FRAME_OBJECT = 0x40000008,
	AUX_MOVEHADNLE_OBJECT = 0x40000010,
	AUX_MOVEHADNLE_ARROW_OBJECT = 0x40000020,
	TOOTH_BASE_OBJECT = 0x40000040,
	TOOTH_VIRTUAL_OBJECT = 0x40000080,
	AUX_TRACING_PLATE_OBJECT = 0x80000000,
	ANY_OBJECT = 0xFFFFFFFF
};

#define TC_OBJECT_TYPE1(className, objectType) \
	virtual ObjectType_e GetObjectType() const { return objectType; } \
	virtual bool IsKindOf(ObjectType_e type) const { return objectType == type; }

#define TC_OBJECT_TYPE2(className, objectType, parentClass)	\
	virtual ObjectType GetObjectType() const { return objectType; } \
	virtual bool IsKindOf(ObjectType_e type) const { return objectType == type ? true : parentClass::IsKindOf(type); }


#define TC_OBJECT_VISIBLE			0x00000001
#define TC_ENTITY_SELECTED			0x00000002
#define TC_ENTITY_TEMPORARY			0x00000004
#define TC_ENTITY_HIGHLIGHT			0x00000008
#define TC_ENTITY_ENABLE			0x00000010

#define TC_VERTEX_FEATURE			0x01000000
#define TC_ENTITY_DELETEED          0x00001000

class MObject
{
public:
	MObject(void);
	MObject(const MObject& object);
	virtual ~MObject(void);
	TC_OBJECT_TYPE1(MObject, SUPER_OBJECT);

public:
	MObject& operator =(const MObject& object);

public:
	BOOL IsVisible() const { return m_state & TC_OBJECT_VISIBLE; }
	BOOL IsSelected() const { return m_state & TC_ENTITY_SELECTED; }
	BOOL IsTemporary() const { return m_state & TC_ENTITY_TEMPORARY; }
	BOOL IsDeleted() const { return m_state & TC_ENTITY_DELETEED; }
	BOOL IsEnable() const { return m_state & TC_ENTITY_ENABLE; }
	BOOL IsHighLight() const { return m_state & TC_ENTITY_HIGHLIGHT; }

	void SetEnable(BOOL isEnable);
	void SetSelected(BOOL isSelect);
	void SetHighLight(BOOL isHighLight);

	void SetVisible(BOOL bVisible);
	void SetTemporary(BOOL isTemporary);
	void SetDeleted(BOOL isDeleted);
	void ResetState(int state, BOOL value);

public:
	virtual void Write(ostream& os, int verTag) const;
	virtual bool Read(ifstream& is, int verTag);

protected:
	UINT m_state;
};

inline void MObject::SetEnable(BOOL isEnable)
{
	if (isEnable)
	{
		m_state = m_state | TC_ENTITY_ENABLE;
	}
	else
	{
		m_state = m_state & (~TC_ENTITY_ENABLE);
	}
}

inline void MObject::SetHighLight(BOOL isHighLight)
{
	if (isHighLight)
	{
		m_state = m_state | TC_ENTITY_HIGHLIGHT;
	}
	else
	{
		m_state = m_state & (~TC_ENTITY_HIGHLIGHT);
	}
}

inline void MObject::SetVisible(BOOL isShow)
{
	if (isShow)
	{
		m_state = m_state | TC_OBJECT_VISIBLE;
	}
	else
	{
		m_state = m_state & (~TC_OBJECT_VISIBLE);
	}
}

inline void MObject::SetSelected(BOOL isSelect)
{
	if (isSelect)
	{
		m_state = m_state | TC_ENTITY_SELECTED;
	}
	else
	{
		m_state = m_state & (~TC_ENTITY_SELECTED);
	}
}


inline void MObject::SetTemporary(BOOL isTemporary)
{
	if (isTemporary)
	{
		m_state = m_state | TC_ENTITY_TEMPORARY;
	}
	else
	{
		m_state = m_state & (~TC_ENTITY_TEMPORARY);
	}
}

inline void MObject::SetDeleted(BOOL isDeleted)
{
	if (isDeleted)
	{
		m_state = m_state | TC_ENTITY_DELETEED;
	}
	else
	{
		m_state = m_state & (~TC_ENTITY_DELETEED);
	}
}

inline void MObject::ResetState(int state, BOOL value)
{
	if (value)
	{
		m_state = m_state | state;
	}
	else
	{
		m_state = m_state & (~state);
	}
}


