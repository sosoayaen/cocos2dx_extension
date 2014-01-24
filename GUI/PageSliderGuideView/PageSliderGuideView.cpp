#include "PageSliderGuideView.h"
#include <limits>

USING_NS_CC;
USING_NS_CC_EXT;

#undef max

#define MAX_DOUBLE std::numeric_limits<double>::max()
#define MAX_FLOAT std::numeric_limits<float>::max()

PageSliderGuideView::PageSliderGuideView():
m_pTableView(NULL)
, m_pArrayData(NULL)
, m_nCurPage(0)
, m_fValidDistance(MAX_FLOAT)
, m_nValidPointSize(5)
{
}

PageSliderGuideView::~PageSliderGuideView()
{
	CC_SAFE_RELEASE_NULL(m_pArrayData);
}

CCTableViewCell* PageSliderGuideView::tableCellAtIndex( CCTableView *table, unsigned int idx )
{
	// throw std::exception("The method or operation is not implemented.");
	CCTableViewCell* pCell = table->dequeueCell();

	if (!pCell)
	{
		pCell = new CCTableViewCell;
		pCell->autorelease();
	}

	if (pCell)
	{
		if (m_pArrayData)
		{
			CCSpriteFrame* pSpriteFrame = dynamic_cast<CCSpriteFrame*>(m_pArrayData->objectAtIndex(idx));
			if (pSpriteFrame)
			{
				CCSprite* pSpriteContent = CCSprite::createWithSpriteFrame(pSpriteFrame);
				if (pSpriteContent)
				{
					pCell->removeAllChildren();
					pSpriteContent->setPosition(ccpMult(ccpFromSize(cellSizeForTable(table)), 0.5f));
					pCell->addChild(pSpriteContent);
				}
			}
		}
	}

	return pCell;
}

unsigned int PageSliderGuideView::numberOfCellsInTableView( CCTableView *table )
{
	unsigned int nRet = 0;
	if (m_pArrayData)
	{
		nRet = m_pArrayData->count();
	}
	return nRet;
}

void PageSliderGuideView::tableCellTouched( CCTableView* table, CCTableViewCell* cell )
{
	// throw std::exception("The method or operation is not implemented.");
	unsigned int idx = cell->getIdx();

	// if (m_pArrayData && m_pArrayData->count() - 1 == idx)
	// {
	// 	// 点击了最后界面，移除此层
	// 	this->endDialog();
	// }
}

void PageSliderGuideView::scrollViewDidScroll( CCScrollView* view )
{
	// throw std::exception("The method or operation is not implemented.");
}

void PageSliderGuideView::scrollViewDidZoom( CCScrollView* view )
{
	// throw std::exception("The method or operation is not implemented.");
}

cocos2d::CCSize PageSliderGuideView::cellSizeForTable( CCTableView *table )
{
	// throw std::exception("The method or operation is not implemented.");
	return this->getContentSize();
}

bool PageSliderGuideView::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!DialogLayer::init());
		
		setValidDistance(CCDirector::sharedDirector()->getWinSize().width*0.1f);

		// 初始化内部的表格
		m_pTableView = CCTableView::create(this, CCDirector::sharedDirector()->getWinSize(), NULL);
		m_pTableView->setDirection(kCCScrollViewDirectionHorizontal);
		m_pTableView->setVerticalFillOrder(kCCTableViewFillTopDown);

		m_pTableView->setDelegate(this);

// 		if (m_pTableView)
// 		{
// 			this->pushScrollView(m_pTableView);
// 		}

		addChild(m_pTableView);

		bRet = true;
	} while (0);

	return bRet;
}

void PageSliderGuideView::onEnter()
{
	DialogLayer::onEnter();
}

void PageSliderGuideView::onExit()
{
	DialogLayer::onExit();
}

void PageSliderGuideView::setImageDataArray( cocos2d::CCArray* pData )
{
	if (pData)
	{
		CC_SAFE_RELEASE_NULL(m_pArrayData);
		m_pArrayData = pData;
		CC_SAFE_RETAIN(m_pArrayData);
		if (m_pTableView)
			m_pTableView->reloadData();
	}
}

PageSliderGuideView* PageSliderGuideView::create( cocos2d::CCArray* pData )
{
	PageSliderGuideView* pRet = new PageSliderGuideView;
	if (pRet && pRet->init())
	{
		pRet->setImageDataArray(pData);
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}


CCRect getBoundingBox(const std::vector<CCPoint> &points)
{
	float minX =  MAX_FLOAT;
	float maxX = -MAX_FLOAT;
	float minY =  MAX_FLOAT; 
	float maxY = -MAX_FLOAT;

	for (std::vector<CCPoint>::const_iterator i = points.begin(); i != points.end(); i++)
	{
		CCPoint point = *i;
		if (point.x < minX)
			minX = point.x;
		if (point.x > maxX)
			maxX = point.x;
		if (point.y < minY)
			minY = point.y;
		if (point.y > maxY)
			maxY = point.y;
	}
	CCRect bounds(minX, minY, (maxX - minX), (maxY - minY));
	return bounds;
}

void PageSliderGuideView::ccTouchEnded( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	DialogLayer::ccTouchEnded(pTouch, pEvent);

	unsigned int nSize = m_pathPoints.size();
	if (nSize >= m_nValidPointSize)
	{
		// DollarRecognizer::RecognitionResult result = m_recognizer.recognize(m_path2D);

		// 比对第一个点和最后一个点的位置是否是向右偏移
		// DollarRecognizer::Rectangle rect = m_recognizer.boundingBox(m_path2D);
		CCRect rect = getBoundingBox(m_pathPoints);
		// const char* pszResult = "Unknow";
		float len = m_pathPoints.back().x - m_pathPoints.front().x;
		
		if (fabs(len) >= fabs(rect.size.width) && fabs(len) >= m_fValidDistance)
		{
			if (len > 0)
			{
				// pszResult = "LeftToRightLine";
				if (m_nCurPage > 0)
				{
					m_nCurPage--;
					if (m_pTableView)
					{
						CCPoint offset = ccp(m_nCurPage * -cellSizeForTable(m_pTableView).width, 0);
						m_pTableView->setContentOffsetInDuration(offset, 0.2f);
						CCLOG("OFFSET (%0.2f, %0.2f)", offset.x, offset.y);
					}
				}
			}
			else
			{
				if (m_pArrayData && m_nCurPage < m_pArrayData->count() - 1)
				{
					m_nCurPage++;
					if (m_pTableView)
					{
						CCPoint offset = ccp(m_nCurPage * -cellSizeForTable(m_pTableView).width, 0);
						m_pTableView->setContentOffsetInDuration(offset, 0.2f);
						CCLOG("OFFSET (%0.2f, %0.2f)", offset.x, offset.y);
					}
				}
				else
				{
					// 关闭引导
					endDialog();
				}	
			}
		}

		CCLOG("----------> len :%0.2f, width: %0.2f, pageid: %d, point size: %d, dis:%0.2f", len, rect.size.width, m_nCurPage, nSize, m_fValidDistance);
		// CCDictionary* pDict = CCDictionary::create();
		// pDict->setObject(ccs(result.name.c_str()), "matchTempleteName");
		// pDict->setObject(ccs(pszResult), "matchTempleteName");
	}
	else if (nSize <= 2)
	{
		if (m_pArrayData && m_pArrayData->count() - 1 == m_nCurPage)
		{
			// 关闭引导
			endDialog();
		}
	}
}

bool PageSliderGuideView::ccTouchBegan( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	bool bRet = DialogLayer::ccTouchBegan(pTouch, pEvent);

	m_pathPoints.clear();
	m_pathPoints.push_back(pTouch->getLocationInView());

	return bRet;
}

void PageSliderGuideView::ccTouchMoved( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	DialogLayer::ccTouchMoved(pTouch, pEvent);

	m_pathPoints.push_back(pTouch->getLocationInView());
}

void PageSliderGuideView::ccTouchCanceled( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	DialogLayer::ccTouchCancelled(pTouch, pEvent);

	m_pathPoints.clear();
}

void PageSliderGuideView::setValidDistance( float fDistance )
{
	m_fValidDistance = fDistance;
}

float PageSliderGuideView::getValidDistance()
{
	return m_fValidDistance;
}