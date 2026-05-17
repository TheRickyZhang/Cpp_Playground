/* Requirements:
* Match first based on price. 
* Within price level, allocate pro-rata rounding down, ie every order matching volume is (vol / total vol) * match vol, rounded down.
* Then, distribute all remaining volume by volume-time priority.
* 
*
* Design: represent bids and asks as:
* map<int, vector<Order>>
* 
* We can also consider accumulating in PriceLevel, 
*
* On first pass, sort by 
* 
* 
*/

