#ifndef __CC_MENU_ITEM_IMAGE_BMF_H__
#define __CC_MENU_ITEM_IMAGE_BMF_H__

#include "cocos2d.h"

class CCMenuItemImageBMF : public cocos2d::CCMenuItemSprite
{
	CC_PROPERTY(cocos2d::CCLabelBMFont*, m_pLabelBMFontNormal, NormalLabelBMFont);
	CC_PROPERTY(cocos2d::CCLabelBMFont*, m_pLabelBMFontSelected, SelectedLabelBMFont);
	CC_PROPERTY(cocos2d::CCLabelBMFont*, m_pLabelBMFontDisabled, DisabledLabelBMFont);
public:
	CCMenuItemImageBMF();
	virtual ~CCMenuItemImageBMF();

	static CCMenuItemImageBMF* create();

	// ��������Image�ĺ���������bmFont
	virtual void setNormalImage(cocos2d::CCNode* pNodeNormal);
	virtual void setSelectedImage(cocos2d::CCNode* pNodeSelected);
	virtual void setDisabledImage(cocos2d::CCNode* pNodeDisabled);

public:
	bool init();

	bool initBMFont();

	/**
	* @brief ����SpriteFrame��ͼƬ���ƣ��ӻ�����ȡ����
	*/
	void setNormalSpriteFrameName(const char* pszFrameName);
	void setSelectedSpriteFrameName(const char* pszFrameName, float fScale = 1.0f);
	void setDisabledSpriteFrameName(const char* pszFrameName);

	/**
	* @brief ֱ������SpriteFrame
	*/
	void setNormalSpriteFrame(cocos2d::CCSpriteFrame* pSpriteFrame);
	void setSelectedSpriteFrame(cocos2d::CCSpriteFrame* pSpriteFrame, float fScale = 1.0f);
	void setDisabledSpriteFrame(cocos2d::CCSpriteFrame* pSpriteFrame);

	/**
	* @brief ����normal��bmf�ַ���
	*/
	void setNormalFntFile(const char* pszFntFile);
	void setSelectedFntFile(const char* pszFntFile);
	void setDisabledFntFile(const char* pszFntFile);

	void setAllFntFile(const char* pszFntFile);

	/** 
	* @brief ���ð�ť����������
	*/
	void setNormalCString(const char* pszContent);
	void setSelectedCString(const char* pszContent);
	void setDisabledCString(const char* pszContent);

	void setAllCString(const char* pszContent);

	/**
	* @brief ����bmf�ֵ�λ��
	*/
	void setNormalBMFontPosition(const cocos2d::CCPoint& point);
	void setSelectedBMFontPosition(const cocos2d::CCPoint& point);
	void setDisabledBMFontPosition(const cocos2d::CCPoint& point);

	void setNormalBMFontPositionRate(const cocos2d::CCPoint& point);
	void setSelectedBMFontPositionRate(const cocos2d::CCPoint& point);
	void setDisabledBMFontPositionRate(const cocos2d::CCPoint& point);
	void setAllBMFontPosistionRate(const cocos2d::CCPoint& point);

protected:

private:
};

#endif // __CC_MENU_ITEM_IMAGE_BMF_H__
