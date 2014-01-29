/**
 * Author: Jason Tou
 * Mail: sosoayaen@gamil.com
 */
#ifndef __PULL_2_REFRESH_TABLE_VIEW_H__
#define __PULL_2_REFRESH_TABLE_VIEW_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#define CC_GETTER_AND_SETTER(varType, memberName, funcName) \
protected: varType memberName;\
public: inline virtual void set##funcName(varType val){memberName = val;}\
public: inline virtual varType get##funcName(){return memberName;}

// status
enum {
	kCCPull2RefreshStatusNormal,
	kCCPull2RefreshStatusPullDownReleaseToRefresh,
	kCCPull2RefreshStatusPullUpReleaseToRefresh
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
	/**
	 * @brief trigger refresh data action
	 * @return bool return true if there is a refresh operation, false means no refresh. it's return true in default
	 */
	virtual bool onPullDownRefresh(Pull2RefreshTableView* table) = 0;
	virtual bool onPullUpRefresh(Pull2RefreshTableView* table) = 0;

	/**
	* @brief when data returned, call this function to update the view
	* @param table
	*/
	virtual void onPullDownRefreshComplete(Pull2RefreshTableView* table);
	virtual void onPullUpRefreshComplete(Pull2RefreshTableView* table);

	/**
	 * @brief trigger when pull down. 
	 * you should update the header loading sprite status(posistion, rotation etc.)
	 * it will be called freqently, do not make a long time operation here
	 * @param table
	 * @param fDistance
	 * @param fThreshold
	 */
	virtual void onPullDownDidScroll(Pull2RefreshTableView* table, float fDistance, float fThreshold) = 0;

	/**
	 * @brief trigger when pull up.
	 * you should update the footer loading sprite status(posistion, rotation etc.)
	 * it will be called freqently, do not make a long time operation here
	 * @param table
	 * @param fDistance
	 * @param fThreshold
	 */
	virtual void onPullUpDidScroll(Pull2RefreshTableView* table, float fDistance, float fThreshold) = 0;
};

class Pull2RefreshTableView : public cocos2d::extension::CCTableView
{
	/** 
	 * ZERO in default. these two variables indicated the onPullDownDidScroll and onPullUpDidScroll
	 * will not be triggered unless pull distance is beyond the offset threshold
	 */
	CC_GETTER_AND_SETTER(float, m_fPullDownOffsetThreshold, PullDownOffsetThreshold);
	CC_GETTER_AND_SETTER(float, m_fPullUpOffsetThreshold, PullUpOffsetThreshold);
	
	/** 
	 * these two variables indicated when onPullDownRefresh and onPullUpRefresh be triggerd
	 */
	CC_GETTER_AND_SETTER(float, m_fPullDownThreshold, PullDownThreshold);
	CC_GETTER_AND_SETTER(float, m_fPullUpThreshold, PullUpThreshold);

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
	* @brief set and get Delegate override the base class
	*/
	void setDelegate(Pull2RefreshTableViewDelegate* pDelegate);
	Pull2RefreshTableViewDelegate* getDelegate();

	/**
	* @brief overwrite the _updateContentSize() to update the header and footer
	*/
	void _updateContentSize();

	// if you want to add another operation than do only reload data when data returned
	// you should override this method
	virtual void onPullDownRefreshComplete();
	virtual void onPullUpRefreshComplete();

	// override this method to implement loading animation
	virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);

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
   	void setFooterNode(cocos2d::CCNode* pNodeFooter);

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
	 * @brief get the distance of pull action, you can override it 
	 */
	virtual float getPullDistance();

protected:
	// enum status	
	int m_nPullActionStatus;

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
