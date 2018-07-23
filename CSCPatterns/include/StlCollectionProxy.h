/*
 * StlCollectionProxy.h
 *
 *  Created on: Jul 23, 2018
 *      Author: wnash
 *
 *    @brief Dummy file to make a shared library containing proxies for the following classes,
 *      which root doesn't like to handle natively. Likely a more intelligent way to handle this
 */

#ifndef CSCPATTERNS_INCLUDE_STLCOLLECTIONPROXY_H_
#define CSCPATTERNS_INCLUDE_STLCOLLECTIONPROXY_H_

#ifdef __MAKECINT__
#pragma link C++ class vector<vector<int> >+;
#pragma link C++ class vector<vector<vector<int> > >+;
#endif

#endif /* CSCPATTERNS_INCLUDE_STLCOLLECTIONPROXY_H_ */
