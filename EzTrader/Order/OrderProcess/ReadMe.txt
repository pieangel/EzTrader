�ֹ� ó�� ����. 
���� ���� �ϴ� �Լ��� �� �Ѱ��� �����ϵ��� �Ѵ�. 
SI ������ �ֹ��� �����ϰų� ó���� �� �׻� ���ο� �ֹ� ��ȣ�� �´�. 
�׸��� ����Ȯ���̳� ���Ȯ�� ���� �ֹ� ��ȣ�� ���߿� �´�. 
�� ������ ���� �ֹ� ��ȣ�� ���� ���Ƿ� ������ �ֹ��̳� ��ҵ� �ֹ��� Ȯ���Ͽ� ����� ��� �Ѵ�. 
�׸��� ���߿� OrderUnfilled Event�� �� ���� ��ҳ� ������ ���� Ȯ�θ� �����Ѵ�. 

��ҳ� ������ �� �� ���� ȭ�鿡�� ���ְ� ���Ȯ���̳� ����Ȯ���� ���߿� ó���ϴ� ������ �Ѵ�. 
�ֹ��� �� ���� �ֹ� ��ü�� ���� ����� ���� ȭ�鿡 ǥ���ϰ� �ֹ� ���¸� Ledge���·� �����Ѵ�.
Ledge������ �ֹ��� ����ϰų� ������ �� ����. 

A. �ֹ� Ȯ�� ó�� ����.
1. ���ǻ翡�� �ֹ� ������ �޴´�.
2. �ֹ������� �Ľ��Ͽ� order_info json ��ü�� �����.
3. order_processor ť�� �ִ´�.
4. order_processor�� ť���� ������ �ֹ��� ó���ϱ� �����Ѵ�.
5. order_info�� ���� ���Ŀ� order_request_id�� �ִ��� �ľ��Ѵ�.
6. order_request_id  ������ �ִ� ��� 
  6.1. order_request_list���� order_request�� ã�� ����. 
  6.2. order_request_list�� �ִ� ���
    6.2.1. order_request�� �������� order��ü�� �����.
    6.2.2. order��  order_request_id, order_no, accepted_count, accepted_time, original_order_no, first_order_no�� �־��ش�. 
    6.2.3. order_account_type�� sub_account �� ��� order_account_type�� �־��ش�.  
  6.3. order_request_list�� ���� ���� 7�ΰ� �ܺ��ֹ��� ó���Ѵ�. 
7. order_request_list�� ���� ��� �� order_request_id ������ ���� ��� - ������ �ܺ� �ֹ�(���� ����ڰ� �ֹ��� ���� �ֹ�â �ܿ� �ٸ� process, �ٸ� ip�ּ� �� �ֹ�)���� ����
  7.1. order_info�� �������� order ��ü�� �����. 
  7.2. order�� account_no, symbol_code, order_no, accepted_count, accepted_time, original_order_no, first_order_no�� �־��ش�.
8. account_no�� ��� account_order_manager�� ã�´�.
9. account_order_manager���� symbol_order_manager�� ã�´�. 
9. symbol_order_manager accepted_order_list�� �߰��� �ش�. 
10. symbol_order_manager buy_order_manager/sell_order_manager�� order�� �߰��� �ش�. 
11. account_accepted_order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 
12. order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 
13. order_account_type�� sub_account �� ���
  13.1. order_request_id ������ main_account_no�� ��� account_order_manager�� ã�´�.
  13.2. account_order_manager���� symbol_order_manager�� ã�´�. 
  13.3. symbol_order_manager accepted_order_list�� �߰��� �ش�. 
  13.4. symbol_order_manager buy_order_manager/sell_order_manager�� order�� �߰��� �ش�. 
  13.5. order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 
  13.6. account_accepted_order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 


B. �ֹ� ü�� ó�� ����.
1. ���ǻ翡�� �ֹ� ������ �޴´�.
2. �ֹ������� �Ľ��Ͽ� order_info json ��ü�� �����.
3. order_processor ť�� �ִ´�.
4. order_processor�� ť���� ������ �ֹ��� ó���ϱ� �����Ѵ�.
5. order_info�� ���� ���Ŀ� order_request_id�� �ִ��� �ľ��Ѵ�.
6. order_request_id  ������ �ִ� ��� 
6.1. order_request_list���� order_request�� ã�� ����. 
  6.2. order_request_list�� �ִ� ���
    6.2.1. order_request�� �������� order��ü�� �����.
    6.2.2. order��  order_request_id, order_no, accepted_count, accepted_time, original_order_no, first_order_no�� �־��ش�. 
    6.2.3. order_account_type�� sub_account �� ��� order_account_type�� �־��ش�.  
  6.3. order_request_list�� ���� ���� 7�ΰ� �ܺ��ֹ��� ó���Ѵ�. 

