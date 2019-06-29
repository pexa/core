// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2019 The Pexa Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef PEXA_QT_PEXAADDRESSVALIDATOR_H
#define PEXA_QT_PEXAADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class PexaAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit PexaAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Pexa address widget validator, checks for a valid pexa address.
 */
class PexaAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit PexaAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // PEXA_QT_PEXAADDRESSVALIDATOR_H
