주문 처리 순서. 
같은 일을 하는 함수는 단 한개만 존재하도록 한다. 
SI 증권은 주문을 변경하거나 처리할 때 항상 새로운 주문 번호가 온다. 
그리고 정정확인이나 취소확인 주문 번호는 나중에 온다. 
이 때 정정된 주문이나 취소된 주문을 확인하여 취소해 줘야 한다. 
취소나 정정을 할 때 먼저 화면에서 없애고 취소확인이나 정정확인은 나중에 처리하는 것으로 한다. 
주문을 낼 때도 주문 객체를 먼저 만들고 먼저 화면에 표시하고 주문 상태를 Ledge상태로 변경한다.
Ledge상태의 주문은 취소하거나 정정할 수 없다. 

주문 확인 처리 순서.
1. 증권사에서 주문 정보를 받는다.
2. 주문정보를 파싱하여 order_info json 객체를 만든다.
3. order_processor 큐에 넣는다.
4. order_processor는 큐에서 꺼내어 주문을 처리하기 시작한다.
5. order_info를 꺼낸 이후에 order_request_id가 있는지 파악한다.
6. order_request_id  정보가 있는 경우 
  6.1 order_request_list에서 order_request를 찾아 본다. 
  6.2 order_request_id가 있는 경우
    6.2.1 order_request를 바탕으로 order객체를 만든다.
    6.2.2 order에 order_id, accepted_count, accepted_time을 넣어준다. 
7. order_request_id 정보가 없는 경우 - 무조건 외부 주문(현재 사용자가 주문을 내는 주문창 외에 다른 process, 다른 ip애서 온 주문)으로 간주
  7.1 order_info를 바탕으로 order 객체를 만든다. 
  7.2 order에 account_no, symbol_code, order_id, accepted_count, accepted_time을 넣어준다.
8. symbol_order_manager accepted_order_list에 추가해 준다. 
9. symbol_order_manager buy_order_manager/sell_order_manager에 order를 추가해 준다. 
10. account_accepted_order_view에 accepted_order_list가 갱신되었다는 event를 보낸다. 
11. order_view에 accepted_order_list가 갱신되었다는 event를 보낸다. 

