#include "CCMenuItemImageBMF.h"

USING_NS_CC;

CCMenuItemImageBMF::~CCMenuItemImageBMF()
{
	CC_SAFE_RELEASE_NULL(m_pLabelBMFontNormal);
	CC_SAFE_RELEASE_NULL(m_pLabelBMFontSelected);
	CC_SAFE_RELEASE_NULL(m_pLabelBMFontDisabled);
}

CCMenuItemImageBMF::CCMenuItemImageBMF():
m_pLabelBMFontNormal(NULL),
m_pLabelBMFontSelected(NULL),
m_pLabelBMFontDisabled(NULL)
{
	m_pLabelBMFontNormal = CCLabelBMFont::create();
	m_pLabelBMFontNormal->retain();

	m_pLabelBMFontSelected = CCLabelBMFont::create();
	m_pLabelBMFontSelected->retain();

	m_pLabelBMFontDisabled = CCLabelBMFont::create();
	m_pLabelBMFontDisabled->retain();
}

void CCMenuItemImageBMF::setNormalFntFile( const char* pszFntFile )
{
	if (m_pLabelBMFontNormal)
	{
		m_pLabelBMFontNormal->setFntFile(pszFntFile);
	}
}

void CCMenuItemImageBMF::setSelectedFntFile( const char* pszFntFile )
{
	if (m_pLabelBMFontSelected)
	{
		m_pLabelBMFontSelected->setFntFile(pszFntFile);
	}
}

void CCMenuItemImageBMF::setDisabledFntFile( const char* pszFntFile )
{
	if (m_pLabelBMFontDisabled)
	{
		m_pLabelBMFontDisabled->setFntFile(pszFntFile);
	}
}


void CCMenuItemImageBMF::setAllFntFile( const char* pszFntFile )
{
	setNormalFntFile(pszFntFile);
	setSelectedFntFile(pszFntFile);
	setDisabledFntFile(pszFntFile);
}

#define SetBMFontString(pBMFont, content) if (pBMFont) \
{\
	if (pBMFont) \
	{\
		pBMFont->setCString(content);\
		if (pBMFont->getParent()) \
		{\
			CCSize size = pBMFont->getParent()->getContentSize();\
			CCSize sizeBMFont = pBMFont->getContentSize();\
			if (size.width*0.9f < sizeBMFont.width)\
			{\
				CCLOG("--------------->parent's size:(%0.2f, %0.2f), bmf size:(%0.2f, %0.2f)", size.width, size.height, sizeBMFont.width, sizeBMFont.height);\
				pBMFont->setScale(size.width*0.9f / sizeBMFont.width);\
			}\
		}\
	}\
}

void CCMenuItemImageBMF::setNormalCString( const char* pszContent )
{
	SetBMFontString(m_pLabelBMFontNormal, pszContent);
}

void CCMenuItemImageBMF::setSelectedCString( const char* pszContent )
{
// 	if (m_pLabelBMFontSelected)
// 	{
// 		m_pLabelBMFontSelected->setCString(pszContent);
// 	}
	SetBMFontString(m_pLabelBMFontSelected, pszContent);
}

void CCMenuItemImageBMF::setDisabledCString( const char* pszContent )
{
// 	if (m_pLabelBMFontDisabled)
// 	{
// 		m_pLabelBMFontDisabled->setCString(pszContent);
// 	}
	SetBMFontString(m_pLabelBMFontDisabled, pszContent);
}

void CCMenuItemImageBMF::setAllCString( const char* pszContent )
{
	setNormalCString(pszContent);
	setSelectedCString(pszContent);
	setDisabledCString(pszContent);
}

CCMenuItemImageBMF* CCMenuItemImageBMF::create()
{
	CCMenuItemImageBMF *pRet = new CCMenuItemImageBMF();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool CCMenuItemImageBMF::init()
{
	return initWithNormalSprite(NULL, NULL, NULL, NULL, NULL);
}

#define SetBMFont(pParentNode, labelBMFont) \
if (pParentNode && labelBMFont) \
{\
	if (labelBMFont->getParent()) labelBMFont->removeFromParent(); \
	labelBMFont->setAnchorPoint(ccp(0.5f, 0.5f)); \
	labelBMFont->setPosition(ccpMult(ccpFromSize(pParentNode->getContentSize()), 0.5f)); \
	pParentNode->addChild(labelBMFont);\
}

void CCMenuItemImageBMF::setNormalImage( CCNode* pNodeNormal )
{
	SetBMFont(pNodeNormal, m_pLabelBMFontNormal);
	CCMenuItemSprite::setNormalImage(pNodeNormal);
}

void CCMenuItemImageBMF::setSelectedImage( CCNode* pNodeSelected )
{
	SetBMFont(pNodeSelected, m_pLabelBMFontSelected);
	CCMenuItemSprite::setSelectedImage(pNodeSelected);
}

void CCMenuItemImageBMF::setDisabledImage( CCNode* pNodeDisabled )
{
	SetBMFont(pNodeDisabled, m_pLabelBMFontDisabled);
	CCMenuItemSprite::setDisabledImage(pNodeDisabled);
}

void CCMenuItemImageBMF::setNormalSpriteFrameName( const char* pszFrameName )
{
	if (pszFrameName)
	{
		setNormalImage(CCSprite::createWithSpriteFrameName(pszFrameName));
	}
}

void CCMenuItemImageBMF::setSelectedSpriteFrameName( const char* pszFrameName, float fScale /*= 1.0f*/ )
{
	if (pszFrameName)
	{
		CCSprite* pSprite = CCSprite::createWithSpriteFrameName(pszFrameName);
		// 这里设置锚点没用。setSelectedImage后会归零
		// pSpriteSelected->setAnchorPoint(ccp(0.5f, 0.5f));
		// 根据放大比例来计算中心点对齐的位移
		// 得到往左下角偏移的距离
		float x = (fScale - 1.0f)*pSprite->getContentSize().width*0.5f;
		float y = (fScale - 1.0f)*pSprite->getContentSize().height*0.5f;
		pSprite->setPosition(ccpSub(CCPointZero, ccp(x, y)));
		pSprite->setScale(fScale);

		setSelectedImage(pSprite);
	}
}

void CCMenuItemImageBMF::setDisabledSpriteFrameName( const char* pszFrameName )
{
	if (pszFrameName)
	{
		setDisabledImage(CCSprite::createWithSpriteFrameName(pszFrameName));
	}
}

void CCMenuItemImageBMF::setNormalLabelBMFont(CCLabelBMFont* pLBMFont)
{
	if (m_pLabelBMFontNormal)
	{
		if (m_pLabelBMFontNormal->getParent())
		{
			m_pLabelBMFontNormal->getParent()->addChild(pLBMFont);
			m_pLabelBMFontNormal->removeFromParent();
		}
		m_pLabelBMFontNormal->release();
	}
	m_pLabelBMFontNormal = pLBMFont;
}

void CCMenuItemImageBMF::setSelectedLabelBMFont(CCLabelBMFont* pLBMFont)
{
	if (m_pLabelBMFontSelected)
	{
		if (m_pLabelBMFontSelected->getParent())
		{
			m_pLabelBMFontSelected->getParent()->addChild(pLBMFont);
			m_pLabelBMFontSelected->removeFromParent();
		}
		m_pLabelBMFontSelected->release();
	}
	m_pLabelBMFontSelected = pLBMFont;
}

void CCMenuItemImageBMF::setDisabledLabelBMFont(CCLabelBMFont* pLBMFont)
{
	if (m_pLabelBMFontDisabled)
	{
		if (m_pLabelBMFontDisabled->getParent())
		{
			m_pLabelBMFontDisabled->getParent()->addChild(pLBMFont);
			m_pLabelBMFontDisabled->removeFromParent();
		}
		m_pLabelBMFontDisabled->release();
	}
	m_pLabelBMFontDisabled = pLBMFont;
}

CCLabelBMFont* CCMenuItemImageBMF::getNormalLabelBMFont()
{
	return m_pLabelBMFontNormal;
}

CCLabelBMFont* CCMenuItemImageBMF::getSelectedLabelBMFont()
{
	return m_pLabelBMFontSelected;
}

CCLabelBMFont* CCMenuItemImageBMF::getDisabledLabelBMFont()
{
	return m_pLabelBMFontDisabled;
}

void CCMenuItemImageBMF::setNormalSpriteFrame( cocos2d::CCSpriteFrame* pSpriteFrame )
{
	setNormalImage(CCSprite::createWithSpriteFrame(pSpriteFrame));
}

void CCMenuItemImageBMF::setSelectedSpriteFrame( cocos2d::CCSpriteFrame* pSpriteFrame, float fScale /*= 1.0f*/ )
{
	CCSprite* pSprite = CCSprite::createWithSpriteFrame(pSpriteFrame);
	// 这里设置锚点没用。setSelectedImage后会归零
	// pSpriteSelected->setAnchorPoint(ccp(0.5f, 0.5f));
	// 根据放大比例来计算中心点对齐的位移
	// 得到往左下角偏移的距离
	float x = (fScale - 1.0f)*pSprite->getContentSize().width*0.5f;
	float y = (fScale - 1.0f)*pSprite->getContentSize().height*0.5f;
	pSprite->setPosition(ccpSub(CCPointZero, ccp(x, y)));
	pSprite->setScale(fScale);
	setSelectedImage(pSprite);
}

void CCMenuItemImageBMF::setDisabledSpriteFrame( cocos2d::CCSpriteFrame* pSpriteFrame )
{
	setDisabledImage(CCSprite::createWithSpriteFrame(pSpriteFrame));
}

void CCMenuItemImageBMF::setNormalBMFontPosition( const cocos2d::CCPoint& point )
{
	if (m_pLabelBMFontNormal)
	{
		m_pLabelBMFontNormal->setPosition(point);
	}
}

void CCMenuItemImageBMF::setSelectedBMFontPosition( const cocos2d::CCPoint& point )
{
	if (m_pLabelBMFontSelected)
	{
		m_pLabelBMFontSelected->setPosition(point);
	}
}

void CCMenuItemImageBMF::setDisabledBMFontPosition( const cocos2d::CCPoint& point )
{
	if (m_pLabelBMFontDisabled)
	{
		m_pLabelBMFontDisabled->setPosition(point);
	}
}

void CCMenuItemImageBMF::setNormalBMFontPositionRate( const cocos2d::CCPoint& point )
{
	if (m_pLabelBMFontNormal && m_pLabelBMFontNormal->getParent())
	{
		CCSize size = m_pLabelBMFontNormal->getParent()->getContentSize();
		m_pLabelBMFontNormal->setPosition(ccp(size.width*point.x, size.height*point.y));
	}
}

void CCMenuItemImageBMF::setSelectedBMFontPositionRate( const cocos2d::CCPoint& point )
{
	if (m_pLabelBMFontSelected && m_pLabelBMFontSelected->getParent())
	{
		CCSize size = m_pLabelBMFontSelected->getParent()->getContentSize();
		m_pLabelBMFontSelected->setPosition(ccp(size.width*point.x, size.height*point.y));
	}
}

void CCMenuItemImageBMF::setDisabledBMFontPositionRate( const cocos2d::CCPoint& point )
{
	if (m_pLabelBMFontDisabled && m_pLabelBMFontDisabled->getParent())
	{
		CCSize size = m_pLabelBMFontDisabled->getParent()->getContentSize();
		m_pLabelBMFontDisabled->setPosition(ccp(size.width*point.x, size.height*point.y));
	}
}

void CCMenuItemImageBMF::setAllBMFontPosistionRate( const cocos2d::CCPoint& point )
{
	setNormalBMFontPositionRate(point);
	setSelectedBMFontPositionRate(point);
	setDisabledBMFontPositionRate(point);
}
