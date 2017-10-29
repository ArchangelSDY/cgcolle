#include "CGColleV1CompositeRule.h"

CGColleV1CompositeRule::~CGColleV1CompositeRule()
{
}

QString CGColleV1CompositeRule::matcher() const
{
    return m_matcher;
}

void CGColleV1CompositeRule::setMatcher(const QString &matcher)
{
    m_matcher = matcher;
}
