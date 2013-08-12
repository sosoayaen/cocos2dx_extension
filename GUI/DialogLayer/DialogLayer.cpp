#include "DialogLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;


DialogLayer::DialogLayer():
    // m_pDefaultMenu(NULL),
	//    mTouchedMenu(false),
	m_pCurrentTouchMenu(NULL),
	m_pCurrentTouchEditBox(NULL),
	m_pCurrentTouchScrollView(NULL),
	m_pVisibleNode(NULL),
    mMenuItemArray(NULL),
	m_pOtherTouchCtl(NULL),
	m_nTouchCount(0)
{
}

DialogLayer::~DialogLayer()
{
}

bool DialogLayer::init()
{
    bool bRet = false;
    
    do {
        CC_BREAK_IF(!CCLayerColor::initWithColor(ccc4(0, 0, 0, 125)));
        
        onInitDialog();
        initMenu();

        bRet = true;
    } while (0);
    
    return bRet;
}

bool DialogLayer::initMenu()
{
    if (mMenuItemArray && mMenuItemArray->count() > 0) {
//        if (!m_pDefaultMenu) {
		// 创建默认按钮
            CCMenu* pDefaultMenu = CCMenu::createWithArray(mMenuItemArray);
            pDefaultMenu->setPosition(CCPointZero);
            addChild(pDefaultMenu);

			// 将当前菜单加入队列
			m_vMenuArrays.push_back(pDefaultMenu);
 //       }
    }
    return true;
}

void DialogLayer::onEnter()
{
    CCLayerColor::onEnter();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority - 1, true);
}

void DialogLayer::onExit()
{
    CCLayerColor::onExit();
	// 自己注销
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

bool DialogLayer::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	m_nTouchCount++;

	m_pCurrentTouchMenu = NULL;
	// 循环判断内部每个按钮来响应触屏事件，第一个响应后跳出循环
	std::vector<CCMenu*>::iterator iterMenu = m_vMenuArrays.begin();
	for (; iterMenu != m_vMenuArrays.end(); ++iterMenu)
	{
		if (*iterMenu && (*iterMenu)->ccTouchBegan(pTouch, pEvent))
		{
			// mTouchedMenu = m_pMenu->ccTouchBegan(pTouch, pEvent);
			// 记录前截获触摸消息的按钮
			m_pCurrentTouchMenu = *iterMenu;
			return true;
		}
	}

	// 循环判断是否是其他控件响应消息，比如editbox之类
	m_pCurrentTouchEditBox = NULL;
	std::vector<CCEditBox*>::iterator iterEditBox = m_vEditBoxArrays.begin();
	for (; iterEditBox != m_vEditBoxArrays.end(); ++iterEditBox)
	{
		if (*iterEditBox && (*iterEditBox)->isVisible() && (*iterEditBox)->ccTouchBegan(pTouch, pEvent))
		{
			m_pCurrentTouchEditBox = *iterEditBox;
			return true;
		}
	}

	// 针对ScrollView
	std::vector<CCScrollView*>::iterator iterScrollView = m_vScrollViewArrays.begin();
	for (; iterScrollView != m_vScrollViewArrays.end(); ++iterScrollView)
	{
		if (*iterScrollView && (*iterScrollView)->isVisible() && (*iterScrollView)->ccTouchBegan(pTouch, pEvent))
		{
			m_pCurrentTouchScrollView = *iterScrollView;
			return true;
		}
	}

	m_pOtherTouchCtl = NULL;
	std::vector<CCLayer*>::iterator iterOtherCtl = m_vOtherCtlArray.begin();
	for (; iterOtherCtl != m_vOtherCtlArray.end(); ++iterOtherCtl)
	{
		if (*iterOtherCtl && (*iterOtherCtl)->isVisible() && (*iterOtherCtl)->ccTouchBegan(pTouch, pEvent))
		{
			m_pOtherTouchCtl = *iterOtherCtl;
			return true;
		}
	}

	// 判断是否点击在显示面板之外的区域
	if (m_pVisibleNode)
	{
		CCPoint touchLocation = pTouch->getLocation();
		CCPoint local = m_pVisibleNode->convertToNodeSpace(touchLocation);

		CCPoint nodePostion = m_pVisibleNode->getPosition();
		CCSize nodeContentSize = m_pVisibleNode->getContentSize();

		CCRect r = CCRectMake( nodePostion.x - nodeContentSize.width * m_obAnchorPoint.x,
			nodePostion.y - nodeContentSize.height * m_obAnchorPoint.y,
			nodeContentSize.width, nodeContentSize.height);
		r.origin = CCPointZero;

		// 如果当前点不包含在面板内，则直接退出当前界面
		if (!r.containsPoint(local))
		{
			if (m_pCurrentTouchScrollView && m_pCurrentTouchScrollView->isDragging())
			{
				// 如果有ScrollView被选中，并且是在拖放状态，则不退出当前对话框
				return true;
			}
			// 结束对话框
			endDialog();
		}	
	}

	// 吞掉消息
    return true;
}

void DialogLayer::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	if (m_pCurrentTouchMenu)
	{
        m_pCurrentTouchMenu->ccTouchMoved(pTouch, pEvent);
    }

	if (m_pCurrentTouchEditBox)
	{
		m_pCurrentTouchEditBox->ccTouchMoved(pTouch, pEvent);
	}

	if (m_pOtherTouchCtl)
	{
		m_pOtherTouchCtl->ccTouchMoved(pTouch, pEvent);
	}

	if (m_pCurrentTouchScrollView)
	{
		m_pCurrentTouchScrollView->ccTouchMoved(pTouch, pEvent);
	}
}

void DialogLayer::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
//	CCLOG("DialogLayer::ccTouchEnded()");
	m_nTouchCount--;

    if (m_pCurrentTouchMenu) 
	{
        m_pCurrentTouchMenu->ccTouchEnded(pTouch, pEvent);
    }
	m_pCurrentTouchMenu = NULL;

	if (m_pCurrentTouchEditBox)
	{
		m_pCurrentTouchEditBox->ccTouchEnded(pTouch, pEvent);
	}
 	m_pCurrentTouchEditBox = NULL;

	if (m_pOtherTouchCtl)
	{
		m_pOtherTouchCtl->ccTouchEnded(pTouch, pEvent);
	}
	m_pOtherTouchCtl = NULL;

	if (m_pCurrentTouchScrollView)
	{
		CCLOG("m_pCurrentTouchScrollView->ccTouchEnded(pTouch, pEvent);");
		m_pCurrentTouchScrollView->ccTouchEnded(pTouch, pEvent);
	}

	if (m_nTouchCount == 0)
	{
		m_pCurrentTouchScrollView = NULL;
	}
}

void DialogLayer::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	if (m_pCurrentTouchMenu) 
	{
		m_pCurrentTouchMenu->ccTouchCancelled(pTouch, pEvent);
	}
	m_pCurrentTouchMenu = NULL;

	if (m_pCurrentTouchEditBox)
	{
		m_pCurrentTouchEditBox->ccTouchCancelled(pTouch, pEvent);
	}
	m_pCurrentTouchEditBox = NULL;

	if (m_pOtherTouchCtl)
	{
		m_pOtherTouchCtl->ccTouchCancelled(pTouch, pEvent);
	}
	m_pOtherTouchCtl = NULL;
	
	if (m_pCurrentTouchScrollView)
	{
		m_pCurrentTouchScrollView->ccTouchCancelled(pTouch, pEvent);
	}
	m_pCurrentTouchScrollView = NULL;
}

void DialogLayer::pushMenuItem( cocos2d::CCMenuItem* pMenuItem )
{
	if (!mMenuItemArray) {
		mMenuItemArray = CCArray::create();
	}

	mMenuItemArray->addObject(pMenuItem);
}

void DialogLayer::pushMenu( cocos2d::CCMenu* pMenu )
{
	if (pMenu != NULL)
	{
		m_vMenuArrays.push_back(pMenu);
	}
}

void DialogLayer::pushOtherTouchedCtl( cocos2d::CCLayer* pOtherCtl )
{
	if (pOtherCtl)
	{
		m_vOtherCtlArray.push_back(pOtherCtl);
	}
}

void DialogLayer::SetVisibleBoard( CCNode* pNode )
{
	m_pVisibleNode = pNode;
}

void DialogLayer::pushEditBox( cocos2d::extension::CCEditBox* pEditBox )
{
	if (pEditBox != NULL)
	{
		m_vEditBoxArrays.push_back(pEditBox);
	}
}

bool DialogLayer::endDialog(cocos2d::CCAction* pAction /*= NULL*/, float fDuration /*= 1.0f*/)
{
	// 如果有动画则执行动画没有则直接删除，这个由子类实现
	removeFromParentAndCleanup(true);
	return true;
}

void DialogLayer::pushScrollView( cocos2d::extension::CCScrollView* pScrollView )
{
	if (pScrollView)
	{
		m_vScrollViewArrays.push_back(pScrollView);
	}
}

bool DialogLayer::popMenu( cocos2d::CCMenu* pMenu )
{
	if (!pMenu)
	{
		return false;
	}

	std::vector<CCMenu*>::iterator iterMenu = m_vMenuArrays.begin();
	for (; iterMenu != m_vMenuArrays.end(); ++iterMenu)
	{
		if (*iterMenu == pMenu)
		{
			m_vMenuArrays.erase(iterMenu);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popMenuByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCMenu*>::iterator iterMenu = m_vMenuArrays.begin();
	for (; iterMenu != m_vMenuArrays.end(); ++iterMenu)
	{
		if ((*iterMenu)->getTag() == nTag)
		{
			m_vMenuArrays.erase(iterMenu);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popEditBoxByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCEditBox*>::iterator iterEditBox = m_vEditBoxArrays.begin();
	for (; iterEditBox != m_vEditBoxArrays.end(); ++iterEditBox)
	{
		if ((*iterEditBox)->getTag() == nTag)
		{
			m_vEditBoxArrays.erase(iterEditBox);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popEditBox( CCEditBox* pEditBox )
{
	if (!pEditBox)
	{
		return false;
	}

	std::vector<CCEditBox*>::iterator iterEditBox = m_vEditBoxArrays.begin();
	for (; iterEditBox != m_vEditBoxArrays.end(); ++iterEditBox)
	{
		if (*iterEditBox == pEditBox)
		{
			m_vEditBoxArrays.erase(iterEditBox);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popScrollView( CCScrollView* pScrollView )
{
	if (!pScrollView)
	{
		return false;
	}

	std::vector<CCScrollView*>::iterator iterScrollView = m_vScrollViewArrays.begin();
	for (; iterScrollView != m_vScrollViewArrays.end(); ++iterScrollView)
	{
		if (*iterScrollView == pScrollView)
		{
			m_vScrollViewArrays.erase(iterScrollView);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popScrollViewByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCScrollView*>::iterator iterScrollView = m_vScrollViewArrays.begin();
	for (; iterScrollView != m_vScrollViewArrays.end(); ++iterScrollView)
	{
		if ((*iterScrollView)->getTag() == nTag)
		{
			m_vScrollViewArrays.erase(iterScrollView);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popOtherTouchedCtl( CCLayer* pOtherCtl )
{
	if (!pOtherCtl)
	{
		return false;
	}

	std::vector<CCLayer*>::iterator iterCtl = m_vOtherCtlArray.begin();
	for (; iterCtl != m_vOtherCtlArray.end(); ++iterCtl)
	{
		if (*iterCtl == pOtherCtl)
		{
			m_vOtherCtlArray.erase(iterCtl);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popOtherTouchedCtrlByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCLayer*>::iterator iterCtl = m_vOtherCtlArray.begin();
	for (; iterCtl != m_vOtherCtlArray.end(); ++iterCtl)
	{
		if ((*iterCtl)->getTag() == nTag)
		{
			m_vOtherCtlArray.erase(iterCtl);
			return true;
		}
	}

	return false;
}
