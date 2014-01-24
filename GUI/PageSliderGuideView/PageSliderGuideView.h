/**
* 此类为简单的滑动导航场景
* 用作新手导航
*/
#ifndef __PAGE_SLIDER_GUIDE_VIEW_H__
#define __PAGE_SLIDER_GUIDE_VIEW_H__

#include "cocos2d.h"
#include "cocos-ext.h"
// #include "RecognizerDialogLayer.h"
#include "DialogLayer.h"
#include <vector>

class PageSliderGuideView : public DialogLayer
, public cocos2d::extension::CCTableViewDataSource
, public cocos2d::extension::CCTableViewDelegate
{
public:
	PageSliderGuideView();
	~PageSliderGuideView();

	static PageSliderGuideView* create(cocos2d::CCArray* pData);
	// CREATE_FUNC(PageSliderGuideView);

	virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex( cocos2d::extension::CCTableView *table, unsigned int idx );

	virtual unsigned int numberOfCellsInTableView( cocos2d::extension::CCTableView *table );

	virtual void tableCellTouched( cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell );

	virtual void scrollViewDidScroll( cocos2d::extension::CCScrollView* view );

	virtual void scrollViewDidZoom( cocos2d::extension::CCScrollView* view );

	virtual cocos2d::CCSize cellSizeForTable( cocos2d::extension::CCTableView *table );

	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

	virtual bool ccTouchBegan( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );

	virtual void ccTouchMoved( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );

	virtual void ccTouchCanceled( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );
	
	virtual void ccTouchEnded( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );

	void setImageDataArray(cocos2d::CCArray* pData);

	/**
	 * @brief 设定手指滑过的X距离作为滑动的距离判断，默认为屏幕的1/10长度
	 * @param fDistance
	 */
	void setValidDistance(float fDistance);

	float getValidDistance();

	virtual bool onInitDialog() {return true;}

	/**
	 * @brief 设定为做手势判断的采样点数目，默认为5个点
	 */
	void setValidPointSize(unsigned int nPoints)
	{
		m_nValidPointSize = nPoints;
	}

	unsigned int getValidPointSize()
	{
		return m_nValidPointSize;
	}


protected:
	
	// 展示新手导航内容的容器
	cocos2d::extension::CCTableView* m_pTableView;

	// 引导图名称数组
	cocos2d::CCArray* m_pArrayData;

	// 当前展示的界面
	unsigned int m_nCurPage;

	// 屏幕触点的路径
	std::vector<cocos2d::CCPoint> m_pathPoints;

	// 算手势有效的距离
	float m_fValidDistance;

	// 确定为手势的采样点数
	unsigned int m_nValidPointSize;
private:

};

#endif // __PAGE_SLIDER_GUIDE_VIEW_H__