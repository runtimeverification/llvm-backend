package org.kframework.backend.llvm.matching

import org.kframework.attributes.{Location, Source}

sealed trait MatchingExceptionType
case object UselessRule extends MatchingExceptionType
case object NonExhaustiveMatch extends MatchingExceptionType

case class MatchingException(t: MatchingExceptionType, message: String, source: Source, location: Location)