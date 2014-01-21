#include "Pull2RefreshTableView.h"

USING_NS_CC;
USING_NS_CC_EXT;

void Pull2RefreshTableViewDelegate::onPullDownRefreshComplete(Pull2RefreshTableView* table)
{
	table->onPullDownRefreshComplete();
}

void Pull2RefreshTableViewDelegate::onPullUpRefreshComplete(Pull2RefreshTableView* table)
{
	table->onPullUpRefreshComplete();
}

Pull2RefreshTableView::Pull2RefreshTableView():
m_pNodeFooter(NULL)
,m_pNodeHeader(NULL)
,m_pActionHeader(NULL)
,m_pActionFooter(NULL)
,m_fPullDownDistance(10000.0f)
,m_fPullUpDistance(10000.0f)
,m_nPullStatus(kCCPull2RefreshStatusNormal)
,m_nPullType(kCCPull2RefreshTypeNone)
{
	m_pHeader = CCNode::create();
	CC_SAFE_RETAIN(m_pHeader);
	m_pFooter = CCNode::create();
	CC_SAFE_RETAIN(m_pFooter);
}

Pull2RefreshTableView::~Pull2RefreshTableView()
{
	// Release the loading node
	// CC_SAFE_RELEASE_NULL(m_pNodeHeader);
	// CC_SAFE_RELEASE_NULL(m_pNodeFooter);
	// CC_SAFE_RELEASE_NULL(m_pActionHeader);
	// CC_SAFE_RELEASE_NULL(m_pActionFooter);

	CC_SAFE_RELEASE(m_pHeader);
	CC_SAFE_RELEASE(m_pFooter);
}

Pull2RefreshTableView* Pull2RefreshTableView::create( CCTableViewDataSource* pDataSource,
		cocos2d::CCSize size,
		cocos2d::CCNode* pContainer /*= NULL*/,
		cocos2d::CCNode* pNodeHeader /*= NULL*/,
		cocos2d::CCNode* pNodeFooter /*= NULL*/)
{
	return Pull2RefreshTableView::create(pDataSource, size, pContainer, pNodeHeader, pNodeFooter, NULL, NULL);
}

Pull2RefreshTableView* Pull2RefreshTableView::create( CCTableViewDataSource* pDataSource,
		cocos2d::CCSize size,
		cocos2d::CCNode* pContainer,
		cocos2d::CCNode* pNodeHeader,
		cocos2d::CCNode* pNodeFooter,
		cocos2d::CCActionInterval* pActionHeader,
		cocos2d::CCActionInterval* pActionFooter )
{
	Pull2RefreshTableView* pRet = new Pull2RefreshTableView();
	if (pRet && pRet->initWithViewSize(size, pContainer))
	{
		pRet->setDataSource(pDataSource);
		pRet->_updateCellPositions();
		pRet->_updateContentSize();

		pRet->setHeaderNode(pNodeHeader);
		if (pNodeHeader)
		{
			// set pullDownDistance in default
			pRet->setPullDownDistance(pNodeHeader->getContentSize().height*2);
			// set HEADER AREA's height in default, you could change it in after
			pRet->setHeaderAreaHeight(pNodeHeader->getContentSize().height);
		}
		pRet->setFooterNode(pNodeFooter);
		if (pNodeFooter) pRet->setPullUpDistance(pNodeFooter->getContentSize().height);
		pRet->setHeaderAction(pActionHeader);
		pRet->setFooterAction(pActionFooter);
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

void Pull2RefreshTableView::onPullDownRefresh()
{
	Pull2RefreshTableViewDelegate* pDelegate = getDelegate();
	pDelegate->onPullDownRefresh(this);
}

void Pull2RefreshTableView::onPullUpRefresh()
{
	Pull2RefreshTableViewDelegate* pDelegate = getDelegate();
	pDelegate->onPullUpRefresh(this);
}

void Pull2RefreshTableView::setDelegate( Pull2RefreshTableViewDelegate* pDelegate )
{
	CCTableView::setDelegate(pDelegate);
}

Pull2RefreshTableViewDelegate* Pull2RefreshTableView::getDelegate()
{
	return (Pull2RefreshTableViewDelegate*)CCTableView::getDelegate();
}

void Pull2RefreshTableView::_updateContentSize()
{
	// update the content size by base function first
	CCTableView::_updateContentSize();

	// updatePositionHeaderNode();
	// updatePositionFooterNode();
	updateHeaderArea();
	updateFooterArea();
}

void Pull2RefreshTableView::onPullDownRefreshComplete()
{
	reloadData();
}

// pull up data is appending at the end,
// so we should set the offset whitch refreshed before
void Pull2RefreshTableView::onPullUpRefreshComplete()
{
	CCPoint offset = getContentOffset();
	reloadData();
	setContentOffset(offset);
}

void Pull2RefreshTableView::setHeaderNode(CCNode* pNodeHeader)
{
	if (pNodeHeader)
	{
		if (m_pNodeHeader && m_pNodeHeader->getParent())
		{
			// remove from the parent
			m_pNodeHeader->removeFromParent();	
		}
		// replace the old node
		m_pNodeHeader = pNodeHeader;
		// add to header Area
		m_pHeader->addChild(m_pNodeHeader);
	}
}

void Pull2RefreshTableView::setFooterNode(CCNode* pNodeFooter)
{
	if (pNodeFooter)
	{
		if (m_pNodeFooter && m_pNodeFooter->getParent())
		{
			// remove from the parent
			m_pNodeFooter->removeFromParent();	
		}
		// replace the old node
		m_pNodeFooter = pNodeFooter;
		// add to footer Area
		m_pHeader->addChild(m_pNodeFooter);
	}
}

void Pull2RefreshTableView::updateHeaderNodePosition()
{
	// update the header node position
	if (m_pNodeHeader && m_pNodeHeader->getParent())
	{
		// set anchor point to center
		m_pNodeHeader->setAnchorPoint(ccp(0.5f, 0.5f));
		// set middle of parent node
		m_pNodeHeader->setPosition(ccpMult(ccpFromSize(m_pNodeHeader->getParent()->getContentSize()), 0.5f));
	}
}

void Pull2RefreshTableView::updateFooterNodePosition()
{
	// update the footer node position
	if (m_pNodeFooter)
	{
		CCSize size = getContentSize();
		CCPoint pos = CCPointZero;

		if (size.height > getViewSize().height)
		{
		//	pos.y = 
		}

		m_pNodeFooter->setPosition(pos);
	}
}

void Pull2RefreshTableView::updateHeaderArea()
{
	// reset the content size
	if (m_pHeader)
	{
		m_pHeader->setContentSize(CCSizeMake(
					// fit the container's width
					getViewSize().width,
					// use the original height whitch setted by setHeaderAreaHeight()
					m_pHeader->getContentSize().height
					));
		m_pHeader->setPosition(ccp(0, getContentSize().height));

		// readd HEADER AREA
		addHeaderArea();
	}
	updateHeaderNodePosition();
}

void Pull2RefreshTableView::setHeaderAreaHeight(float fHeight)
{
	if (m_pHeader) 
	{
		m_pHeader->setContentSize(CCSizeMake(
				m_pHeader->getContentSize().width,
				fHeight
				));

		// update the area
		updateHeaderArea();
	}
}

void Pull2RefreshTableView::setFooterAreaHeight(float fHeight)
{
	if (m_pFooter)
	{
		m_pFooter->setContentSize(CCSizeMake(
					m_pFooter->getContentSize().width,
					fHeight
					));
		
		// update the area
		updateFooterArea();
	}
}

void Pull2RefreshTableView::updateFooterArea()
{
	// reset the content size
	if (m_pFooter)
	{
		m_pFooter->setContentSize(CCSizeMake(
					// fit the container's width
					getViewSize().width,
					// use the original height whitch setted by setFooterAreaHeight()
					m_pFooter->getContentSize().height
					));
		m_pFooter->setPosition(ccp(0, getContentSize().height));

		// readd FOOTER AREA
		addFooterArea();
	}
	updateFooterNodePosition();
}

float Pull2RefreshTableView::getPullDistance()
{
	CCPoint offset = getContentOffset();
	float fDistance = 0.0f;

	switch (m_nPullType)
	{
		case kCCPull2RefreshTypeDown:
			fDistance = fabs(offset.y + getContainer()->getContentSize().height - getViewSize().height);
			break;
		case kCCPull2RefreshTypeUp:
			// TODO: count pull up distance
			break;
	}

	return fDistance;
}

void Pull2RefreshTableView::scrollViewDidScroll(CCScrollView* view)
{
	float degree = 180.0f;
	// judge offset to trigger refresh
	float fDis = getPullDistance();

	switch(m_nPullType)
	{
		case kCCPull2RefreshTypeDown:
			if (fDis > m_fPullDownDistance)
			{
				if (m_nPullStatus == kCCPull2RefreshStatusPullDownReleaseToRefresh)
				{
					// reset stauts
					m_nPullStatus = kCCPull2RefreshStatusNormal;
					this->onPullDownRefresh();
				}
			}
			else
			{
				degree = 180*fDis/m_fPullDownDistance;
			}
			
			CCLOG("degree:%0.2f, fDis::%0.2f, dis:%0.2f", degree, fDis, m_fPullDownDistance);
			if (m_pNodeHeader)
			{
				CCNode* loadNode = (CCNode*)m_pNodeHeader->getChildren()->lastObject();
				if (loadNode)
				{
					loadNode->setRotation(degree);
				}
			}
			break;
		case kCCPull2RefreshTypeUp:
			// TODO: handle Pull Up Refresh
			// this->onPullUpRefresh();
			break;
	}

	CCTableView::scrollViewDidScroll(view);
}

void Pull2RefreshTableView::setDirection(CCScrollViewDirection eDirection)
{
	// only support verticial direction
	if (eDirection != kCCScrollViewDirectionVertical)
		CCLOG("Pull2RefreshTableView only support vertical direction.");
	m_eDirection = kCCScrollViewDirectionVertical;

}

void Pull2RefreshTableView::setContainer(CCNode* pContainer)
{
	CCScrollView::setContainer(pContainer);
	// add HEADER AREA
	addHeaderArea();
	// TODO: add FOOTER AREA
	addFooterArea();
}

void Pull2RefreshTableView::addHeaderArea()
{
	if (m_pHeader)
	{
	   if (m_pHeader->getParent())
		{
			m_pHeader->removeFromParent();
		}
		getContainer()->addChild(m_pHeader);
	}
}

void Pull2RefreshTableView::addFooterArea()
{
	if (m_pFooter)
	{
		if (m_pFooter->getParent())
		{
			m_pFooter->removeFromParent();
		}
		getContainer()->addChild(m_pHeader);
	}
}

void Pull2RefreshTableView::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCTableView::ccTouchEnded(pTouch, pEvent);

	// calculate the offset if it trigger refresh distance
	CCPoint offset = getContentOffset();
	CCPoint pointRelease = getContainer()->convertToNodeSpace(pTouch->getLocation());
	CCLOG("pointRelease (%0.2f, %0.2f)", pointRelease.x, pointRelease.y);

	// pull DOWN or UP
	m_nPullType = kCCPull2RefreshTypeNone;
	if (pointRelease.y > m_pointPullStart.y)
	{
		m_nPullType = kCCPull2RefreshTypeUp;
	}
	else if (pointRelease.y < m_pointPullStart.y)
	{
		m_nPullType = kCCPull2RefreshTypeDown;
	}

	float fDistance = getPullDistance();

	// set status
	switch(m_nPullType)
	{
		case kCCPull2RefreshTypeUp:
			if (fDistance > m_fPullUpDistance)
			{
				m_nPullStatus = kCCPull2RefreshStatusPullUpReleaseToRefresh;
			}
			break;
		case kCCPull2RefreshTypeDown:
			if (fDistance > m_fPullDownDistance)
			{
				m_nPullStatus = kCCPull2RefreshStatusPullDownReleaseToRefresh;
			}
			break;
		default:
			m_nPullStatus = kCCPull2RefreshStatusNormal;
			break;
	}

	m_pointPullStart = CCPointZero;
}

bool Pull2RefreshTableView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool bRet = CCTableView::ccTouchBegan(pTouch, pEvent);
	if (bRet)
	{
		m_pointPullStart = getContainer()->convertToNodeSpace(pTouch->getLocation());
		CCLOG("m_pointPullStart (%0.2f, %0.2f)", m_pointPullStart.x, m_pointPullStart.y);
	}
	return bRet;
}

void Pull2RefreshTableView::ccTouchCancelled(CCTouch* pTouch, CCEvent *pEvent)
{
	CCTableView::ccTouchCancelled(pTouch, pEvent);

	m_nPullType = kCCPull2RefreshTypeNone;
	m_nPullStatus = kCCPull2RefreshStatusNormal;
}
