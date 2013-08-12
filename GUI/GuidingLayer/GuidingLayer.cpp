#include "GuidingLayer.h"
#include "EasyUseApi.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace bailin;

#define TAG_ID_TIPS_NODE 100

// 默认启用
bool bailin::GuidingLayer::m_bEnable = true;

static bailin::GuidingLayer* g_sGuidingLayer = NULL;

bool bailin::GuidingLayer::onInitDialog()
{
	return true;
}

bool bailin::GuidingLayer::init()
{
	do
	{
		CC_BREAK_IF(!DialogLayer::init());

		// 默认高亮光圈移动时间为0.5秒
		m_fMotionTime = 0.5f;

		m_nPos = 0;

		// 默认拥有焦点
		m_bFocused = true;

		// 步骤指针回到开始
		m_iterCurrent = m_listGuidingStep.begin();

		// 使本身层的颜色透明
		setOpacity(0);

		// 增加一个新的CCLayerColor
		m_pMaskLayer = CCLayerColor::create(ccc4(0, 0, 0, 128));
		m_pMaskLayer->setAnchorPoint(CCPointZero);
		m_pMaskLayer->setContentSize(this->getContentSize());

		// 把增加一个Node层作为被裁剪的对象层
		m_pClipper = CCClippingNode::create();
		m_pStencilLayer = CCNode::create();

		// 添加被裁剪层
		m_pClipper->addChild(m_pMaskLayer);
		// 添加裁剪模板层
		m_pClipper->setStencil(m_pStencilLayer);

		// 镂空透明，可见后面的元素
		m_pClipper->setInverted(true);
		m_pClipper->setAlphaThreshold(0.05f);
		m_pClipper->setContentSize(m_pMaskLayer->getContentSize());

		// 设定位置
		m_pClipper->setAnchorPoint(CCPointZero);
		m_pClipper->setPosition(CCPointZero);
		addChild(m_pClipper);

		// 高亮光圈
		m_pHole = CCSprite::create("ccbResources/common/light.png");
		m_pHole->setScale(10);
		m_pHole->setPosition(ccpMult(ccpFromSize(this->getContentSize()), 0.5f));
		m_pStencilLayer->addChild(m_pHole);

		return true;

	} while(0);

	return false;
}

void bailin::GuidingLayer::onEnter()
{
	DialogLayer::onEnter();
}

void bailin::GuidingLayer::onExit()
{
	DialogLayer::onExit();

//	CC_SAFE_RELEASE_NULL(g_sGuidingLayer);
}

void bailin::GuidingLayer::AppendGuidingStep( const GUIDING_STEP& gd_step )
{
	m_listGuidingStep.push_back(gd_step);

	m_iterCurrent = m_listGuidingStep.begin();

	int nPos = m_nPos;
	// 重新定位
	while (nPos)
	{
		m_iterCurrent++;
		nPos--;
	}
}

void bailin::GuidingLayer::InsertGuidingStep( const GUIDING_STEP& gd_step )
{

}

void bailin::GuidingLayer::NextStep()
{
	// 如果不启用，则直接返回
	if (!m_bEnable)
	{
		return;
	}

	// 重新获取焦点
	ResetFocus();

	// 如果没有动作则直接移除本层
	if (m_iterCurrent == m_listGuidingStep.end())
	{
		CCActionInterval* pAction = CCSequence::create(
			CCSpawn::create(CCFadeOut::create(m_fMotionTime), CCScaleTo::create(m_fMotionTime, 3), NULL),
			CCCallFunc::create(this, callfunc_selector(GuidingLayer::ActionCallbackRemoveSelf)),
			NULL
			);
//		CC_SAFE_RELEASE_NULL(g_sGuidingLayer);
		m_pHole->stopAllActions();
		m_pHole->runAction(pAction);

		return;
	}

	if (m_iterCurrent != m_listGuidingStep.end())
	{
		// 移除当前裁剪模板层的数据
//		m_pStencilLayer->removeAllChildrenWithCleanup(true);

		if (getChildByTag(TAG_ID_TIPS_NODE))
		{
			removeChildByTag(TAG_ID_TIPS_NODE);
		}

		// 移除监听
		CCLOG(typeid(m_iterCurrent->pTouchedCtl).name());

		if (m_iterCurrent != m_listGuidingStep.begin())
		{
			m_iterCurrent--;
			int nType = m_iterCurrent->nCtrlType;
			switch(nType)
			{
			case TOUCH_CTRL_TYPE_MENU:
				popMenu(dynamic_cast<CCMenu*>(m_iterCurrent->pTouchedCtl));
				break;
			case TOUCH_CTRL_TYPE_EDITBOX:
				popEditBox(dynamic_cast<CCEditBox*>(m_iterCurrent->pTouchedCtl));
				break;
			case TOUCH_CTRL_TYPE_SCROLLVIEW:
				popScrollView(dynamic_cast<CCScrollView*>(m_iterCurrent->pTouchedCtl));
				break;
			case TOUCH_CTRL_TYPE_OTHERCTL:
				popOtherTouchedCtl(dynamic_cast<CCLayer*>(m_iterCurrent->pTouchedCtl));
				break;
			}
			m_iterCurrent++;
		}

		// 添加监听
		switch(m_iterCurrent->nCtrlType)
		{
		case TOUCH_CTRL_TYPE_MENU:
			pushMenu(dynamic_cast<CCMenu*>(m_iterCurrent->pTouchedCtl));
			break;
		case TOUCH_CTRL_TYPE_EDITBOX:
			pushEditBox(dynamic_cast<CCEditBox*>(m_iterCurrent->pTouchedCtl));
			break;
		case TOUCH_CTRL_TYPE_SCROLLVIEW:
			pushScrollView(dynamic_cast<CCScrollView*>(m_iterCurrent->pTouchedCtl));
			break;
		case TOUCH_CTRL_TYPE_OTHERCTL:
			pushOtherTouchedCtl(dynamic_cast<CCLayer*>(m_iterCurrent->pTouchedCtl));
			break;
		}

		// 添加提示对象
		if (m_iterCurrent->pTipsNode)
		{
			m_iterCurrent->pTipsNode->setTag(TAG_ID_TIPS_NODE);
			addChild(m_iterCurrent->pTipsNode);

			// 如果有提示动作则增加
			if (m_iterCurrent->pTipsAction)
			{
				m_iterCurrent->pTipsNode->runAction(m_iterCurrent->pTipsAction);
			}
		}

		// 拷贝一份对象，并且复制其大小，位置，添加到当前的模板层
		// 如果没有给出范围的节点，则直接用触摸节点做大小的比对
		CCNode* pNode = m_iterCurrent->pRectCtl ? m_iterCurrent->pRectCtl : m_iterCurrent->pTouchedCtl;
		CCAssert(pNode, "GuidingLayer m_iterCurrent->pRectCtl and m_iterCurrent->pTouchedCtl must not be null both");
		if(!pNode)
		{
			return;
		}
		// 得到对角线长度
		CCSize size = pNode->getContentSize();
		float fDiameter = sqrtf(pow(size.width, 2) + pow(size.height, 2));

		CCPoint pos = pNode->getPosition();
		pos = EasyUseApi::getNodeCenterPoint(pNode);

		// 坐标系转换
		pos = pNode->getParent()->convertToWorldSpace(pos);
		pos = m_pStencilLayer->convertToNodeSpace(pos);

		// 高光移动效果
		CCActionInterval* pAction = CCSpawn::create(
			CCMoveTo::create(m_fMotionTime, pos),
			CCScaleTo::create(m_fMotionTime, fDiameter / m_pHole->getContentSize().width),
			NULL
			);

		m_pHole->stopAllActions();
		m_pHole->runAction(pAction);

		m_nPos++;
		++m_iterCurrent;
	}
}

void bailin::GuidingLayer::PreviousStep()
{

}

void bailin::GuidingLayer::InitCurrIterator()
{
	m_iterCurrent = m_listGuidingStep.begin();
}

GuidingLayer* bailin::GuidingLayer::sharedGuidingLayer()
{
	if (!g_sGuidingLayer)
	{
		g_sGuidingLayer = GuidingLayer::create();
	}

	return g_sGuidingLayer;
}

void bailin::GuidingLayer::EnableGuide( bool bEnable )
{
	m_bEnable = bEnable;
}

bool bailin::GuidingLayer::ccTouchBegan( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	DialogLayer::ccTouchBegan(pTouch, pEvent);

	if (m_iterCurrent != m_listGuidingStep.end() && m_iterCurrent->bClickOutside)
	{
		// 点击非响应区域也执行下一步操作
		NextStep();
	}

	return true;
}

bailin::GuidingLayer::~GuidingLayer()
{
//	CC_SAFE_RELEASE_NULL(m_pHole);
}

void bailin::GuidingLayer::SetMaskOpacity(GLubyte opacity)
{
	m_pMaskLayer->setOpacity(opacity);
}

void bailin::GuidingLayer::SetMaskColor3B( cocos2d::ccColor3B color )
{
	m_pMaskLayer->setColor(color);
}

void bailin::GuidingLayer::SetHightLightMoveTime( float fTime )
{
	m_fMotionTime = fTime;
}

void bailin::GuidingLayer::ActionCallbackRemoveSelf()
{
	removeFromParent();
}

void bailin::GuidingLayer::ReleaseFocus()
{
	if (!m_bFocused)
	{
		return;
	}

	// 删除 提示文字
	removeChildByTag(TAG_ID_TIPS_NODE);

	// 释放触摸
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);

	// 得到屏幕的对角线
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	float fDiameter = sqrtf(pow(winSize.width, 2) + pow(winSize.height, 2));

	CCPoint pos = ccpMult(ccpFromSize(winSize), 0.5f);
	// 高亮变大
	CCActionInterval* pAction = CCSpawn::create(
		CCMoveTo::create(m_fMotionTime, pos),	// 居中移动
		CCScaleTo::create(m_fMotionTime, fDiameter/m_pHole->getContentSize().width),
		NULL
		);
	m_pHole->stopAllActions();
	m_pHole->runAction(pAction);

	// 失去焦点
	m_bFocused = false;
}

void bailin::GuidingLayer::ResetFocus()
{
	if (!m_bFocused)
	{
		CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority - 1, true);
		m_bFocused = true;
	}
}

void bailin::GuidingLayer::JumpNextStep()
{
	if (m_iterCurrent == m_listGuidingStep.end())
	{
		// 结束
		NextStep();
		return;
	}

	m_iterCurrent++;
}
