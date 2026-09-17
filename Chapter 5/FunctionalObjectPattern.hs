{-# LANGUAGE ExistentialQuantification #-}

-- A functional complement to Visitor: the typeclass lists all operations,
-- while each instance keeps one concrete type's implementations together.
module FunctionalObjectPattern where

class ExpressionObject a where
  evaluate :: a -> Double
  display :: a -> String

data Literal = Literal Double

instance ExpressionObject Literal where
  evaluate (Literal value) = value
  display (Literal value) = show value

data Add left right = Add left right

instance (ExpressionObject left, ExpressionObject right) =>
    ExpressionObject (Add left right) where
  evaluate (Add left right) = evaluate left + evaluate right
  display (Add left right) =
    "(" ++ display left ++ " + " ++ display right ++ ")"

-- Existential packaging allows values with different concrete expression types
-- to be placed behind one interface when heterogeneous storage is needed.
data AnyExpression = forall a. ExpressionObject a => AnyExpression a

evaluateAny :: AnyExpression -> Double
evaluateAny (AnyExpression expression) = evaluate expression

displayAny :: AnyExpression -> String
displayAny (AnyExpression expression) = display expression

example :: AnyExpression
example = AnyExpression (Add (Literal 1) (Literal 2))
