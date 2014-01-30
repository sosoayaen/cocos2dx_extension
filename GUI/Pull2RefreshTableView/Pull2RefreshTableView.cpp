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
,m_fPullDownThreshold(10000.0f)
,m_fPullUpThreshold(10000.0f)
,m_nPullActionStatus(kPull2RefreshStatusNormal)
,m_nPullType(kPull2RefreshTypeNone)
,m_fPullDownOffsetThreshold(0.0f)
,m_fPullUpOffsetThreshold(0.0f)
,m_bLockTableAtRefreshData(false)
{
	m_pHeader = CCNode::create();
	CC_SAFE_RETAIN(m_pHeader);
	m_pFooter = CCNode::create();
	CC_SAFE_RETAIN(m_pFooter);
}

Pull2RefreshTableView::~Pull2RefreshTableView()
{
	CC_SAFE_RELEASE(m_pHeader);
	CC_SAFE_RELEASE(m_pFooter);
}

Pull2RefreshTableView* Pull2RefreshTableView::create( CCTableViewDataSource* pDataSource,
		cocos2d::CCSize size,
		cocos2d::CCNode* pContainer /*= NULL*/,
		cocos2d::CCNode* pNodeHeader /*= NULL*/,
		cocos2d::CCNode* pNodeFooter /*= NULL*/)
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
			// set pullDownThreshold in default
			pRet->setPullDownThreshold(pNodeHeader->getContentSize().height*2);
			// set HEADER AREA's height in default, you could change it in after
			pRet->setHeaderAreaHeight(pNodeHeader->getContentSize().height);
		}
		pRet->setFooterNode(pNodeFooter);
		if (pNodeFooter)
		{
			pRet->setPullUpThreshold(pNodeFooter->getContentSize().height*2);
			pRet->setFooterAreaHeight(pNodeFooter->getContentSize().height);
		}
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
	if (pDelegate && pDelegate->onPullDownRefresh(this))
	{
		if (m_pHeader)
		{
			// set header node show in offset
			// CCLOG("view.height=%0.2f, container.height=%0.2f, header.height=%0.2f, offset.y=%0.2f", getViewSize().height, getContentSize().height, m_pHeader->getContentSize().height, getContentOffset().y);
			stopScrollAnimation();
			CCPoint offset = ccp(0, getViewSize().height - getContentSize().height - m_pHeader->getContentSize().height);
			this->setContentOffsetInDuration(offset, 0.2f);
		}
	}
}

void Pull2RefreshTableView::onPullUpRefresh()
{
	Pull2RefreshTableViewDelegate* pDelegate = getDelegate();
	if (pDelegate && pDelegate->onPullUpRefresh(this))
	{
		if (m_pFooter)
		{
			stopScrollAnimation();
			float fOffsetY = getViewSize().height - getContentSize().height;
			fOffsetY = fOffsetY < 0 ? 0 : fOffsetY;
			CCPoint offset = ccp(0, fOffsetY + m_pFooter->getContentSize().height);
			this->setContentOffsetInDuration(offset, 0.2f);
		}
	}
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
	m_nPullActionStatus = kPull2RefreshStatusNormal;
	reloadData();
}

// pull up data is appending at the end,
// so we should set the offset whitch refreshed before
void Pull2RefreshTableView::onPullUpRefreshComplete()
{
	m_nPullActionStatus = kPull2RefreshStatusNormal;
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
		m_pFooter->addChild(m_pNodeFooter);
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
	if (m_pNodeFooter && m_pNodeFooter->getParent())
	{
		// set anchor point to center
		m_pNodeFooter->setAnchorPoint(ccp(0.5f, 0.5f));
		// set middle of parent node
		m_pNodeFooter->setPosition(ccpMult(ccpFromSize(m_pNodeFooter->getParent()->getContentSize()), 0.5f));
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
		// anchor point is top-left
		m_pFooter->setAnchorPoint(ccp(0, 1.0f));
		m_pFooter->setPosition(ccp(0, MIN(0, getContentSize().height - getViewSize().height)));
		// CCLOG("updateFooterArea, m_pFooter's position (%0.2f, %0.2f), height = %0.2f", m_pFooter->getPositionX(), m_pFooter->getPositionY(), getContentSize().height);

		// readd FOOTER AREA
		addFooterArea();
	}
	// updateFooterNodePosition();
}

float Pull2RefreshTableView::getPullDistance()
{
	const CCPoint offset = getContentOffset();
	const CCSize tSize = getContentSize();
	const CCSize vSize = getViewSize();
	float fDistance = 0.0f;

	switch (m_nPullType)
	{
		case kPull2RefreshTypeDown:
			// if offset.y is less than container.height - view.height
			// means pull down header
			if (offset.y < vSize.height - tSize.height)
				fDistance = fabs(offset.y + tSize.height - vSize.height) - m_fPullDownOffsetThreshold;
		   	// fDistance = (fDistance < 0) ? 0 : fDistance;
			break;
		case kPull2RefreshTypeUp:
			{
				// if the view's height is large than container's height
				// means the original position.y is large than ZERO
				// so we should sub the position.y when calculating the distance
				// and if we sub ZERO will not affect the result
				float fOffsetY = tSize.height - vSize.height > 0 ? 0 : vSize.height - tSize.height;
				if (offset.y > fOffsetY)
				{
					fDistance = offset.y - fOffsetY - m_fPullUpOffsetThreshold;
					// CCLOG("kPull2RefreshTypeUp, offset.y=%0.2f, fOffsetY=%0.2f, fDistance=%0.2f", offset.y, fOffsetY, fDistance);
				}
			}
			break;
		default:
			break;
	}

	return (fDistance < 0) ? 0 : fDistance;
}

void Pull2RefreshTableView::scrollViewDidScroll(CCScrollView* view)
{
	// judge offset to trigger refresh
	float fDis = getPullDistance();
	Pull2RefreshTableViewDelegate* pDelegate = getDelegate();

	switch(m_nPullType)
	{
		case kPull2RefreshTypeDown:
			// if (m_nPullActionStatus == kPull2RefreshStatusPullDownReleaseToRefresh)
			// {
			// 	// reset stauts
			// 	m_nPullActionStatus = kPull2RefreshStatusNormal;
			// }
			if (pDelegate)
			{
				pDelegate->onPullDownDidScroll(this, fDis, m_fPullDownThreshold);
			}
			break;
		case kPull2RefreshTypeUp:
			// if (m_nPullActionStatus == kPull2RefreshStatusPullUpReleaseToRefresh)
			// {
			// 	// reset status
			// 	m_nPullActionStatus = kPull2RefreshStatusNormal;
			// }
			if (pDelegate)
			{
				pDelegate->onPullUpDidScroll(this, fDis, m_fPullUpThreshold);
			}
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
	// add FOOTER AREA
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
		getContainer()->addChild(m_pFooter);
	}
}

void Pull2RefreshTableView::setLockRefreshTable(bool bLock)
{
	m_bLockTableAtRefreshData = bLock;
}

bool Pull2RefreshTableView::isLockRefreshTable()
{
	return m_bLockTableAtRefreshData;
}

bool Pull2RefreshTableView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool bRet = CCTableView::ccTouchBegan(pTouch, pEvent);
	if (bRet)
	{
		m_pointPullStart = getContainer()->convertToNodeSpace(pTouch->getLocation());
		// CCLOG("m_pointPullStart (%0.2f, %0.2f)", m_pointPullStart.x, m_pointPullStart.y);
	}
	return bRet;
}

void Pull2RefreshTableView::ccTouchMoved(CCTouch* pTouch, CCEvent *pEvent)
{
	CCTableView::ccTouchMoved(pTouch, pEvent);

	if (!isLockRefreshTable() || m_nPullActionStatus == kPull2RefreshStatusNormal)
	{
		CCPoint point = getContainer()->convertToNodeSpace(pTouch->getLocation());
		float fOffsetY = point.y - m_pointPullStart.y;
		float fDis = fabs(fOffsetY);
	  
		// change the pull type
		m_nPullType = kPull2RefreshTypeNone;
		if (fOffsetY > 0)
		{
			m_nPullType = kPull2RefreshTypeUp;
		}
		else if (fOffsetY < 0)
		{
			m_nPullType = kPull2RefreshTypeDown;
		}
	}
}

void Pull2RefreshTableView::ccTouchCancelled(CCTouch* pTouch, CCEvent *pEvent)
{
	CCTableView::ccTouchCancelled(pTouch, pEvent);

	m_nPullType = kPull2RefreshTypeNone;
	m_nPullActionStatus = kPull2RefreshStatusNormal;
}

void Pull2RefreshTableView::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCTableView::ccTouchEnded(pTouch, pEvent);

	// calculate the offset if it trigger refresh distance
	CCPoint offset = getContentOffset();

	float fDistance = getPullDistance();

	// set status
	switch(m_nPullType)
	{
		case kPull2RefreshTypeUp:
			if (fDistance > m_fPullUpThreshold)
			{
				m_nPullActionStatus = kPull2RefreshStatusPullUpReleaseToRefresh;
				this->onPullUpRefresh();
			}
			break;
		case kPull2RefreshTypeDown:
			if (fDistance > m_fPullDownThreshold)
			{
				m_nPullActionStatus = kPull2RefreshStatusPullDownReleaseToRefresh;
				this->onPullDownRefresh();
			}
			break;
		default:
			m_nPullActionStatus = kPull2RefreshStatusNormal;
			break;
	}

	m_pointPullStart = CCPointZero;
}
