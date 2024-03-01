package org.kframework.backend.llvm.matching;

import org.kframework.attributes.Location;
import org.kframework.attributes.Source;

import java.util.Optional;

public class MatchingException extends Throwable {
    MatchingExceptionType type;
    String message;
    Optional<Source> source;
    Optional<Location> location;

    public MatchingException(MatchingExceptionType type, String message, Optional<Source> source, Optional<Location> location) {
        this.type = type;
        this.message = message;
        this.source = source;
        this.location = location;
    }

    public MatchingException(MatchingExceptionType type, String message, Source source, Location location) {
        this(type, message, Optional.of(source), Optional.of(location));
    }

    public MatchingException(MatchingExceptionType type, String message) {
        this(type, message, Optional.empty(), Optional.empty());
    }

    public MatchingExceptionType getType() {
        return type;
    }

    @Override
    public String getMessage() {
        return message;
    }

    public Optional<Source> getSource() {
        return source;
    }

    public Optional<Location> getLocation() {
        return location;
    }
}
