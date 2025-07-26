

// LLACE IR follows reverse polish notation
// We utilize this stack based format for fast parsing and interpretation of this IR.
// The RPN IR will not suffer any problmes in optimization potential either utilzing clear control flow graphing and Single Static Assignment variables to create def-use chains.


/*
int main() {
  int x = 10;
  int y = 15;

  int a = 0;

  if (x > 5 || x == 0) { // x >= 0 is not used to showcase complex comparisons in ir
    a = 1;
  } else if (x < 15) {
    a = 2;
  } else {
    a = -1;
  }

  int z = add(x, y);

  return 0;
}

#main {
  // Entry block - variable initialization
  @entry: {
    i32(10) %x.0 =
    i32(15) %y.0 =
    i32(0) %a.0 =
    
    // Evaluate complete condition: (x > 5 || x == 0)
    %x.0 i32(5) > %cond1 =
    i32(0) !! %cond2 = // !! if zero, ! if not zero
    %cond1 %cond2 or %if_condition =
    
    %if_condition @block_then @block_elif_test branch
  }

  // Then branch: a = 1
  @block_then: {
    i32(1) %a.1 =
    @block_merge jmp/1/0
  }

  // Elif condition test
  @block_elif_test: {
    %x.0 i32(15) < %elif_condition =
    %elif_condition @block_elif @block_else branch
  }

  // Elif branch: a = 2
  @block_elif: {
    i32(2) %a.2 =
    @block_merge jmp
  }

  // Else branch: a = -1
  @block_else: {
    i32(-1) %a.3 =
    @block_merge jmp
  }

  // Merge point with phi node
  @block_merge: {
    %a.1 %a.2 %a.3 phi/3/1 %a.final =
    %x.0 %y.0 add %z.0 =
    i32(0) ret/1
  }

}
*/
