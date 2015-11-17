# arithmetic-in-xml

Evaluation of aritmetic expression encoded in xml.

For example, following xml tree is interpreted as (5+(2-1))/(10/2), and as a result 1.2 is printed.

     <Division>
          <Addition>
               <int>5</int>
               <Subtraction>
                    <int>2</int>
                    <int>1</int>
               </Subtraction>
          </Addition>
          <Division>
               <int>10</int>
               <int>2</int>
          </Division>
     </Division>

