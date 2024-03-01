package org.kframework.backend.llvm.matching

import org.kframework.attributes.{Location, Source}

import java.util.Optional

sealed trait MatchingExceptionType
case object UselessRule extends MatchingExceptionType
case object NonExhaustiveMatch extends MatchingExceptionType
case object CompilerError extends MatchingExceptionType
case object InternalError extends MatchingExceptionType

case class MatchingException(t: MatchingExceptionType, message: String, source: Optional[Source], location: Optional[Location]) extends Throwable