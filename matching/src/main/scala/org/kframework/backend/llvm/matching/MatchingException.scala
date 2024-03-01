package org.kframework.backend.llvm.matching

import java.util.Optional
import org.kframework.attributes.Location
import org.kframework.attributes.Source

sealed trait MatchingExceptionType
case object UselessRule        extends MatchingExceptionType
case object NonExhaustiveMatch extends MatchingExceptionType
case object CompilerError      extends MatchingExceptionType
case object InternalError      extends MatchingExceptionType

case class MatchingException(
    t: MatchingExceptionType,
    message: String,
    source: Optional[Source],
    location: Optional[Location]
) extends Throwable
