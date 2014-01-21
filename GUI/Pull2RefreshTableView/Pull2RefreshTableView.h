/**
 * Author: Jason Tou
 * Mail: sosoayaen@gamil.com
 */
#ifndef __PULL_2_REFRESH_TABLE_VIEW_H__
#define __PULL_2_REFRESH_TABLE_VIEW_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#define REPLACE_AUTORELEASE_OBJ(M_OBJ, OBJ) \
	if (OBJ) {CC_SAFE_RELEASE_NULL(M_OBJ); M_OBJ = OBJ; CC_SAFE_RETAIN(M_OBJ);}

// set property and get property in simply way
#define CC_PROPERTY_METHOD(TYPE, PROP, METHOD) \
protected: TYPE m_p##PROP;\
public: virtual void set##METHOD(TYPE p##PROP) {REPLACE_AUTORELEASE_OBJ(m_p##PROP, p##PROP);}\
public: virtual TYPE get##METHOD() {return m_p##PROP;}

// add the node to container in CCSrollView
#define CONTAINER_ADD_CHILD(NODE) \
	if (NODE) \
	{\
		if (NODE->getParent()) \
		{\
			NODE->removeFromParent();\
		}\
		NODE->setAnchorPoint(cocos2d::CCPointZero);\
		NODE->setContentSize(cocos2d::CCSizeMake(getContentSize().width, NODE->getContentSize().height));\
		getContainer()->addChild(NODE);\
	}

#define CC_PROPERTY_METHOD_ADD_TO_CONTAINER(TYPE, PROP, METHOD)\
protected: TYPE m_p##PROP;\
protected: virtual void updatePosition##METHOD();\
public: virtual void set##METHOD(TYPE p##PROP) {REPLACE_AUTORELEASE_OBJ(m_p##PROP, p##PROP); CONTAINER_ADD_CHILD(m_p##PROP); updatePosition##METHOD();}\
public: virtual TYPE get##METHOD() {return m_p##PROP;}

// status
enum {
	kCCPull2RefreshStatusNormal,
	kCCPull2RefreshStatusPullDownReleaseToRefresh,	// 
	kCCPull2RefreshStatusPullUpReleaseToRefresh,
	kCCPull2RefreshStatusPullDown,
	kCCPull2RefreshStatusPullUp
};

// pull type whitch means uesr trigger a pull up or pull down release
enum {
	kCCPull2RefreshTypeNone,
	kCCPull2RefreshTypeUp,
	kCCPull2RefreshTypeDown
};

class Pull2RefreshTableView;

class Pull2RefreshTableViewDelegate : public cocos2d::extension::CCTableViewDelegate
{
public:
	virtual void onPullDownRefresh(Pull2RefreshTableView* table) = 0;
	virtual void onPullUpRefresh(Pull2RefreshTableView* table) = 0;

	/**
	* @brief when data returned, call this function to update the view
	*/
	virtual void onPullDownRefreshComplete(Pull2RefreshTableView* table);
	virtual void onPullUpRefreshComplete(Pull2RefreshTableView* table);
};

class Pull2RefreshTableView : public cocos2d::extension::CCTableView
{
	// Node header
	// CC_PROPERTY_METHOD_ADD_TO_CONTAINER(cocos2d::CCNode*, NodeHeader, HeaderNode);
	// Node footer
	// CC_PROPERTY_METHOD_ADD_TO_CONTAINER(cocos2d::CCNode*, NodeFooter, FooterNode);
	// CC_PROPERTY_METHOD(cocos2d::CCActionInterval*, ActionHeader, HeaderAction);
	// CC_PROPERTY_METHOD(cocos2d::CCActionInterval*, ActionFooter, FooterAction);
public:
	Pull2RefreshTableView();
	~Pull2RefreshTableView();

	/**
	* @brief create with a loading animation node
	* @param pDataSource like CCTableView's param
	* @param size the view size
	* @param pContainer the container of cells
	* @parem pNodeHeader when drop down the list, it show loading status
	*                    this node only need supply height, width will be setted auto
	* @param pNodeFooter when pull up the list, it show loading status
	*/
	static Pull2RefreshTableView* create(cocos2d::extension::CCTableViewDataSource* pDataSource,
		cocos2d::CCSize size,
		cocos2d::CCNode* pContainer = NULL,
		cocos2d::CCNode* pNodeHeader = NULL,
		cocos2d::CCNode* pNodeFooter = NULL);

	/**
	* @brief create with a loading animation node
	* @param pDataSource like CCTableView's param
	* @param size the view size
	* @param pContainer the container of cells
	* @parem pNodeHeader when drop down the list, it show loading status
	*                    this node only need supply height, width will be setted auto
	* @param pNodeFooter when pull up the list, it show loading status
	*/
	static Pull2RefreshTableView* create(cocos2d::extension::CCTableViewDataSource* pDataSource,
		cocos2d::CCSize size,
		cocos2d::CCNode* pContainer,
		cocos2d::CCNode* pNodeHeader,
		cocos2d::CCNode* pNodeFooter,
		cocos2d::CCActionInterval* pActionHeader,
		cocos2d::CCActionInterval* pActionFooter);

	/**
	* @brief set and get Delegate override the base class
	*/
	void setDelegate(Pull2RefreshTableViewDelegate* pDelegate);
	Pull2RefreshTableViewDelegate* getDelegate();

	/**
	* @brief overwrite the _updateContentSize() to update the header and footer
	*/
	void _updateContentSize();

	// if you want to add other operation than do only reload data when data returned
	// you should override this method
	virtual void onPullDownRefreshComplete();
	virtual void onPullUpRefreshComplete();

	// override this method to implement loading animation
	virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);

	void setPullDownDistance(float fDistance)
	{
		m_fPullDownDistance = fDistance;
	}
	
	float getPullDownDistance()
	{
		return m_fPullDownDistance;
	}

	void setPullUpDistance(float fDistance)
	{
		m_fPullUpDistance = fDistance;
	}

	float getPullUpDistance()
	{
		return m_fPullUpDistance;
	}

	/**
	 * @brief update the header area's content size and header node position
	 */
	void updateHeaderArea();

	/**
	 * @brief update the footer area's content size and header node position
	 */
	void updateFooterArea();

	/**
	 * @brief set the height of header Area whitch contains header node
	 * it will call updateHeaderArea() inside
	 */
	void setHeaderAreaHeight(float fHeight);

	/**
	 * @brief set the height of FOOTER AREA whitch contans footer node
	 * it will call updateHeaderArea() inside
	 * @param fHiehgt
	 */
	void setFooterAreaHeight(float fHeight);

	/**
	 * @brief override the setDirection, only support vertical direction
	 */
	virtual void setDirection(cocos2d::extension::CCScrollViewDirection eDirection);

    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

	/**
	 * @brief set the header node whitch could perform loading animation
	 * after call this function, you should comfirm whether if the HEADER AREA's height is correct. you could call setHeaderAreaHeight to fix the height value.
	 * @param pNodeHeader
	 */
   	void setHeaderNode(cocos2d::CCNode* pNodeHeader);
   	void setFooterNode(cocos2d::CCNode* pNodeHeader);

	/**
	 * @brief update the HEADER NODE's position
	 */
	void updateHeaderNodePosition();
	void updateFooterNodePosition();

	/**
	 * @brief overwirte this method to add HEADER AREA to container
	 */
	void setContainer(cocos2d::CCNode* pContainer);
protected:
	/**
	* @brief Call it when pull down refresh complete, it will end the animation
	*/
	virtual void onPullDownRefresh();

	/**
	* @brief Call it when pull up refresh complete, it will end the animation
	*/
	virtual void onPullUpRefresh();
	
	/**
	 * @brief add HEADER AREA to Container
	 * call it in the setContainer and updateHeaderArea
	 */
	virtual void addHeaderArea();

	virtual void addFooterArea();

	/**
	 * @brief 
	 */
	virtual float getPullDistance();

protected:
	// release distance to judge whether is trigger refresh call
	float m_fPullDownDistance;
	float m_fPullUpDistance;

	// enum status	
	int m_nPullStatus;

	// ccTouchBegan record the point
	cocos2d::CCPoint m_pointPullStart;

	// indicate pull is DOWN or UP
	int m_nPullType;

	/**
	 * the header and footer node area whitch contains the real header and footer node
	 */
	cocos2d::CCNode* m_pHeader;
	cocos2d::CCNode* m_pFooter;

	/**
	 * real header node, it was created outside and you should control it in callback function
	 */
	cocos2d::CCNode* m_pNodeHeader;
	cocos2d::CCNode* m_pNodeFooter;
};

#endif // __PULL_2_REFRESH_TABLE_VIEW_H__
