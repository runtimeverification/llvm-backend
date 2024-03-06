package org.kframework.backend.llvm.matching;

import com.runtimeverification.k.kore.Pattern;

import java.util.Optional;

public final class MatchingException extends Throwable {
    public enum Type {
        USELESS_RULE,
        NON_EXHAUSTIVE_MATCH,
        INTERNAL_ERROR,
        COMPILER_ERROR,
    }

    Type type;
    String message;
    Optional<Source> source;
    Optional<Location> location;
    Optional<Pattern> pattern;

    private MatchingException(Type type, String message, Optional<Source> source, Optional<Location> location, Optional<Pattern> pattern) {
        this.type = type;
        this.message = message;
        this.source = source;
        this.location = location;
        this.pattern = pattern;
    }

    public MatchingException(Type type, String message, Optional<Source> source, Optional<Location> location, Pattern pattern) {
        this(type, message, source, location, Optional.of(pattern));
    }


    public MatchingException(Type type, String message, Optional<Source> source, Optional<Location> location) {
        this(type, message, source, location, Optional.empty());
    }

    public MatchingException(Type type, String message, Source source, Location location) {
        this(type, message, Optional.of(source), Optional.of(location));
    }

    public MatchingException(Type type, String message) {
        this(type, message, Optional.empty(), Optional.empty());
    }

    public Type getType() {
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

    public Optional<Pattern> getPattern() {
        return pattern;
    }
}
