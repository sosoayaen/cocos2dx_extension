/**
* ����Ϊ�򵥵Ļ�����������
* �������ֵ���
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
	 * @brief �趨��ָ������X������Ϊ�����ľ����жϣ�Ĭ��Ϊ��Ļ��1/10����
	 * @param fDistance
	 */
	void setValidDistance(float fDistance);

	float getValidDistance();

	virtual bool onInitDialog() {return true;}

	/**
	 * @brief �趨Ϊ�������жϵĲ�������Ŀ��Ĭ��Ϊ5����
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
	
	// չʾ���ֵ������ݵ�����
	cocos2d::extension::CCTableView* m_pTableView;

	// ����ͼ��������
	cocos2d::CCArray* m_pArrayData;

	// ��ǰչʾ�Ľ���
	unsigned int m_nCurPage;

	// ��Ļ�����·��
	std::vector<cocos2d::CCPoint> m_pathPoints;

	// ��������Ч�ľ���
	float m_fValidDistance;

	// ȷ��Ϊ���ƵĲ�������
	unsigned int m_nValidPointSize;
private:

};

#endif // __PAGE_SLIDER_GUIDE_VIEW_H__